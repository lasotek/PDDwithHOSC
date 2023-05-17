using Symbolics
# using SymbolicUtils
using Plots
using Colors
using LaTeXStrings
using Printf
import AMRVW;
const A = AMRVW;

gr(dpi=600);
# pgfplotsx();

out_file_name = "symb_amp_low_C1.txt"

open(out_file_name, "w") do f
    println(f, "Path: $out_file_name")
    println(f, "Results")
end

function append_to_file(content)
    open(out_file_name, "a") do f
        println(f)
        println(f, content)
    end
end


# SymbolicUtils.show_simplified[] = true


@variables g_m r_π r_bb r_o C_π C_μ R_E R_E2 R_B1 R_B2 R_B R_c R_L R_C f C_E2 C_1 C_2 R_S s

com_values = Dict(
    R_E => 280,
    R_E2 => 280,
    R_B1 => 150e3,
    R_B2 => 56e3,
    R_B => 150e3 * 56e3 / (150e3 + 56e3),
    R_C => 1.3e3,
    R_L => 3.3e3,
    R_c => 1.3e3 * 3.3e3 / (1.3e3 + 3.3e3),
    g_m => 1.94e-01,
    r_π => 3.51e+03,
    r_bb => 3.30,
    r_o => 1.49e+04,
    C_π => 1.20e-10,
    C_μ => 2.40e-12,
    C_E2 => 750e-6,
    C_1 => 1e-6,
    C_2 => 1e-6,
    R_S => 300,
    # s => 2π * 1e3 * im,
    # f => 1e3,
)


function s_expand(symbolic_poly_s)
    D = Differential(s)
    # res = Dict()
    res = []
    # symbolic_poly_s = symbolic_poly_s_i
    f0 = substitute.(symbolic_poly_s, (Dict(s => 0),))[1]
    # res[0] = const_v[1]
    append!(res, f0)
    df = expand_derivatives(D(symbolic_poly_s))
    power = 1
    while !iszero(df)
        fi = substitute.(df, (Dict(s => 0),))[1]
        append!(res, fi)
        power += 1
        df = expand_derivatives(D(df))/power
    end
    return res
end
# function s_expand(symbolic_poly_s_i, limit_power::Int64=100)
#     # res = Dict()
#     res = []
#     symbolic_poly_s = symbolic_poly_s_i
#     const_v = simplify.(substitute.(symbolic_poly_s, (Dict(s => 0),)))
#     # res[0] = const_v[1]
#     append!(res, const_v[1])
#     s_power::Int64 = 1
#     while s_power <= limit_power
#         # symbolic_poly_s = simplify.(expand.(symbolic_poly_s - const_v[1]) / s)
#         symbolic_poly_s = symbolic_poly_s - const_v[1]
#         symbolic_poly_s /= s
#         symbolic_poly_s = simplify.(symbolic_poly_s)
#         if iszero(symbolic_poly_s)
#             break
#         end
#         const_v = simplify.(substitute.(symbolic_poly_s, (Dict(s => 0),)))
#         # res[s_power] = const_v[1]
#         append!(res, const_v[1])
#         s_power += 1
#     end
#     return res

# end

function sym_to_complex_f(symbolic_expanded::Vector, return_float::Bool=false)

    function res_fun(f::Number)
        s_expandes = symbolic_expanded
        # @show s_expandes
        real_part = 0
        imag_part = 0
        for (index, con) in enumerate(s_expandes)
            power = index - 1
            if power % 2 == 0
                real_part += (-1)^(power / 2) * con * f^power * (2π)^power
            else
                imag_part += (-1)^((power - 1) / 2) * con * f^power * (2π)^power
            end
        end
        if return_float
            return real_part.val + im * imag_part.val
        else
            return real_part + im * imag_part
        end
    end
    return res_fun
end

# function multicolor_xticks!(p, main_color, new_color, new_xticks, new_xlabels)
#     # p = Plots.current()
#     xticks, xlabels = Plots.xticks(p)[1]
#     yl = Plots.ylims(p)
#     colors = fill(main_color, length(xticks))
#     new_colors = fill(new_color, length(new_xticks))
#     xticks!(xticks, fill(" ", length(xticks)))
#     y0 = @. zero(xticks) + yl[1] - 0.1 * (yl[2] - yl[1])
#     y0N = @. zero(new_xticks) + yl[1] - 0.1 * (yl[2] - yl[1]) - 3
#     append!(xticks, new_xticks)
#     append!(xlabels, new_xlabels)
#     append!(colors, new_colors)
#     append!(y0, y0N)
#     [annotate!(xi, yi, text(latexstring(li), 9, ci)) for (xi, yi, li, ci) in zip(xticks, y0, xlabels, colors)]
#     return Plots.current()
# end

function transform_vector(v::Vector{T}, δ::Float64)::Vector{Float64} where {T<:Num}
    n = length(v)
    # v_to_map = iseven(n) ? v[2:end] : v
    if (iseven(n))
        v_to_map = v[2:end]
        n = n - 1
        d = 1.0
    else
        v_to_map = v
        d = 1.0 + δ
    end
    res = map(enumerate(v_to_map)) do (i, x)
        if iseven(i)
            0
        elseif (i == n)
            x.val * d
        else
            -x.val * δ
        end
    end
    return res
end


function get_characteristic_fs(symbolic_poly_s_c::Vector; δ=0.31)
    vl = length(symbolic_poly_s_c)
    start_a = 1
    while (start_a < vl) && iszero(symbolic_poly_s_c[start_a])
        start_a += 1
    end
    vl -= start_a - 1
    symbolic_poly_s = symbolic_poly_s_c[start_a:end]
    if (vl == 0)
        return [], nothing, nothing
    end
    if (vl ==2 || vl == 1)
        if (vl == 1)
            f1 = 1 / symbolic_poly_s[1].val / 2 / π
        elseif (vl == 2)
            f1 = abs(symbolic_poly_s[1] / symbolic_poly_s[2]).val / 2 / π
        end
        if (f1 < 0.0)
            f1 *= -1
        end
        start = 10.0^(floor(Int16, log10(f1)) - 2)
        stop = 10.0^(ceil(Int16, log10(f1)) + 2)
        return [f1 / 10, f1, 10 * f1], start, stop
    end
    res, start, stop = get_characteristic_fs(symbolic_poly_s[1:end-1])
    if (!iszero(symbolic_poly_s[end]))
        # is_even_power = (vl - 1) % 2 == 0
        eq = transform_vector(symbolic_poly_s, δ)
        # eq = map(x -> -x.val * δ, symbolic_poly_s[1:end-1])
        # append!(eq, symbolic_poly_s[end].val)
        sol = A.roots(eq)
        rli = findall(x -> isreal(x) && real(x) > 0, sol)
        foreach(x -> append!(res, [real(x) / 2 / π]), sol[rli])
    end
    if (isempty(res))
        return [], nothing, nothing
    end
    min_value = minimum(res)
    max_value = maximum(res)
    start = 10.0^(floor(Int16, log10(min_value)) - 1)
    stop = 10.0^(ceil(Int16, log10(max_value)) + 1)
    return res, start, stop
end

function first_order_s_presentation(name::AbstractString, symbolic_poly_s::Vector, lat_name::AbstractString)

    if length(symbolic_poly_s) < 2
        # println("Error: length(symbolic_poly_s) < 2")
        return
    end
    if lat_name == empty
        lat_name = name
    end
    char_points, start, stop = get_characteristic_fs(symbolic_poly_s)
    # AssertionError(length(char_points) >= 0 && findall(x -> isreal(x) && x < 0, char_points) == [])
    char_point_close_tics_pows = map(x -> round(Int16, log10(x)), char_points)
    # char_point_close_tics = map(x -> 10.0^x, char_point_close_tics_pows)
    # phase45 = 45
    append_to_file("$name has corner frequency: $char_points[2]")
    T(f) = sym_to_complex_f(symbolic_poly_s, true)(f)
    # modul(f) = 20log10(abs(T(f)))
    modul(f) = log10(abs(T(f)))
    args(f) = angle(T(f)) * 180 / π
    # v_line_pos = [f_g1 f_g f_g2]
    v_line_pos = char_points
    h_line_pos_modul = map(x -> modul(x), v_line_pos)
    h_line_pos_args = map(x -> args(x), v_line_pos)
    t1 = [zip(v_line_pos, h_line_pos_modul)...]
    t2 = [zip(v_line_pos, h_line_pos_args)...]
    append_to_file("Special points in Module: (f,|v|): $t1")
    append_to_file("Special points in Phase: (f,φ): $t2")
    v_line_labels = [L"0.1f_c", L"f_c", L"10f_c"]
    hv_line_labels = [latexstring("f_{o$(i)}") for i in 4:length(v_line_pos)]
    # append!(v_line_labels, hv_line_labels)
    start_pow = floor(Int16, log10(start))
    end_pow = ceil(Int16, log10(stop))
    ticks_pos = [10.0^p for p in start_pow:end_pow if (p ∉ char_point_close_tics_pows)]
    ticks_pos_empty = map(x -> 10.0^x, char_point_close_tics_pows)
    ticks_pos_labels = [latexstring("10^{$(p)}") for p in start_pow:end_pow if (p ∉ char_point_close_tics_pows)]
    ticks_pos_labels_empty = fill(" ", length(ticks_pos_empty))
    append!(ticks_pos, ticks_pos_empty)
    append!(ticks_pos, v_line_pos)
    append!(ticks_pos_labels, ticks_pos_labels_empty)
    append!(ticks_pos_labels, v_line_labels, hv_line_labels)


    # append!(ticks_pos, [10.0^f_g_pow, 10.0^f_g_pow1, 10.0^f_g_pow2])
    # append!(ticks_pos_labels, [" ", " ", " "])
    # append!(ticks_pos, v_line_pos)
    # append!(ticks_pos_labels, v_line_labels)
    p1 = plot(modul, start, stop, xlabel="\$f\$", label="", xscale=:log10, color=:red, xgrid=:true, ygrid=:true, legend=:topleft, legendfont=font(10), tickfont=font(10), size=(600, 400))
    plot!(p1, title="Module $(latexstring(lat_name))")
    plot!(p1, seriestype="vline", v_line_pos, label="", color=:blue, linestyle=:dash, linewidth=0.7)
    # plot!(p1, seriestype="hline", [log10(abs(symbolic_poly_s[1].val) * sqrt(2))], label="", color=:green, linestyle=:dash, linewidth=0.7)
    plot!(p1, seriestype="hline", h_line_pos_modul, label="", color=:green, linestyle=:dash, linewidth=0.7)
    plot!(p1, xticks=(ticks_pos, ticks_pos_labels))
    ytisks1, _ = Plots.yticks(p1)[1]
    ylabels1 = [latexstring("10^{$(y)}") for y in ytisks1]
    yticks!(ytisks1, ylabels1)
    # multicolor_xticks!(p1, :black, :red, v_line_pos, v_line_labels)

    p2 = plot(args, start, stop, xlabel="\$f\$", label="", xscale=:log10, color=:red, xgrid=:true, ygrid=:true, legend=:topleft, legendfont=font(10), tickfont=font(10), size=(600, 400))
    plot!(p2, title=("Phase $(latexstring(lat_name))"))
    plot!(p2, seriestype="vline", v_line_pos, label="", color=:blue, linestyle=:dash, linewidth=0.7)
    # plot!(p2, seriestype="hline", [phase45], label="", color=:green, linestyle=:dash, linewidth=0.7)
    plot!(p2, seriestype="hline", h_line_pos_args, label="", color=:green, linestyle=:dash, linewidth=0.7)
    # ticks_pos = [10.0^p for p in p_b:p_e]
    # ticks_pos_labels = ["10^{$p}" for p in p_b:p_e]
    plot!(p2, xticks=(ticks_pos, ticks_pos_labels))
    ytisks2, ylabels2 = Plots.yticks(p2)[1]
    ylabels3 = [latexstring("$(y)°") for y in ylabels2]
    yticks!(ytisks2, ylabels3)
    # multicolor_xticks!(p2, :black, :red, v_line_pos, v_line_labels)
    p = plot(p1, p2, layout=(2, 1))
    savefig(p, "$name.png")
    savefig(p, "$name.svg")
    savefig(p, "$name.pdf")
    return start, stop, v_line_pos
end


function division_presentation(name::AbstractString,
    lat_name::AbstractString,
    symbolic_poly_s_num::Vector,
    symbolic_poly_s_den::Vector,
    v_lines_num::Vector,
    v_lines_den::Vector,
    f_range::Vector;
    dbels=false)
    if (length(f_range) != 2)
        println("Error: length(f_range) != 2")
        return
    end

    if (f_range[2] < f_range[1])
        println("Error: f_range[2]<f_range[1]")
        return
    end

    f_g_pow1 = floor(Int16, log10(f_range[1]))
    f_g_pow2 = ceil(Int16, log10(f_range[2]))

    f_b = 10.0^f_g_pow1
    f_e = 10.0^f_g_pow2
    Numerator(f) = sym_to_complex_f(symbolic_poly_s_num, true)(f)
    Denominator(f) = sym_to_complex_f(symbolic_poly_s_den, true)(f)
    T(f) = Numerator(f) / Denominator(f)
    # modul(f) = 20log10(abs(T(f)))
    modul(f) = abs(T(f))
    moduldb(f) = 20 * log10(abs(T(f)))
    args(f) = angle(T(f)) * 180 / π
    # v_line_pos = [f_g1 f_g f_g2]
    # v_line_labels = [L"0.1f_g", L"f_g", L"10f_g"]
    # ticks_pos = [10.0^p for p in p_b:p_e if (p != f_g_pow && p != f_g_pow1 && p != f_g_pow2)]
    # append!(ticks_pos, [10.0^f_g_pow, 10.0^f_g_pow1, 10.0^f_g_pow2])
    # ticks_pos_labels = [latexstring("10^{$(p)}") for p in p_b:p_e if (p != f_g_pow && p != f_g_pow1 && p != f_g_pow2)]
    # append!(ticks_pos_labels, [" ", " ", " "])
    # append!(ticks_pos, v_line_pos)
    # append!(ticks_pos_labels, v_line_labels)
    if (dbels)
        p1 = plot(moduldb, f_b / 10, f_e * 10, xlabel="\$f\$", label="", xscale=:log10, color=:red, xgrid=:true, ygrid=:true, legend=:topleft, legendfont=font(10), tickfont=font(10), size=(600, 400))
    else
        p1 = plot(modul, f_b / 10, f_e * 10, xlabel="\$f\$", label="", xscale=:log10, yscale=:log10, color=:red, xgrid=:true, ygrid=:true, legend=:topleft, legendfont=font(10), tickfont=font(10), size=(600, 400))
    end
    plot!(p1, title="Module $(latexstring(lat_name))")
    # h_lines_num = map(x -> dbels ? moduldb(x) : modul(x), v_lines_num)
    h_lines_num = map(x -> dbels ? moduldb(x) : modul(x), v_lines_num)
    h_lines_den = map(x -> dbels ? moduldb(x) : modul(x), v_lines_den)
    t1_n = [zip(v_lines_num, h_lines_num)...]
    t1_d = [zip(v_lines_den, h_lines_den)...]
    append_to_file("Special points in Module from the numerator: (f,|v| $(dbels ? "dB" : "")): $t1_n")
    append_to_file("Special points in Module from the denominator: (f,|v| $(dbels ? "dB" : "")): $t1_d")
    # append_to_file("Special points in Phase: (f,φ): $t2")

    h_lines_den = map(x -> dbels ? moduldb(x) : modul(x), v_lines_den)
    # start_pow = floor(Int16, log10(f_range[1]))
    # stop_pow = ceil(Int16, log10(f_range[2]))
    ticks_pos = [10.0^p for p in f_g_pow1:f_g_pow2]
    ticks_pos_labels = [latexstring("10^{$(p)}") for p in f_g_pow1:f_g_pow2]
    plot!(p1, seriestype="vline", v_lines_num, label="", color=:deepskyblue, linestyle=:dash, linewidth=0.7)
    plot!(p1, seriestype="vline", v_lines_den, label="", color=:indigo, linestyle=:dash, linewidth=0.7)
    # plot!(p1, seriestype="hline", [log10(abs(symbolic_poly_s[1].val) * sqrt(2))], label="", color=:green, linestyle=:dash, linewidth=0.7)
    plot!(p1, xticks=(ticks_pos, ticks_pos_labels))
    yticks1, yticks1_labels = Plots.yticks(p1)[1]
    ylabels1 = [latexstring("$(y) $(dbels ? "dB" : "")") for y in yticks1_labels]
    yticks!(yticks1, ylabels1)
    # multicolor_xticks!(p1, :black, :red, v_line_pos, v_line_labels)

    h_lines_p_num = map(x -> args(x), v_lines_num)
    h_lines_p_den = map(x -> args(x), v_lines_den)
    t1_p_n = [zip(v_lines_num, h_lines_p_num)...]
    t1_p_d = [zip(v_lines_den, h_lines_p_den)...]
    append_to_file("Special points in Phase from the numerator: (f,φ): $t1_p_n")
    append_to_file("Special points in Phase from the denominator: (f,φ): $t1_p_d")
    p2 = plot(args, f_b / 10, f_e * 10, xlabel="\$f\$", label="", xscale=:log10, color=:red, xgrid=:true, ygrid=:true, legend=:topleft, legendfont=font(10), tickfont=font(10), size=(600, 400))
    plot!(p2, seriestype="vline", v_lines_num, label="", color=:deepskyblue, linestyle=:dash, linewidth=0.7)
    plot!(p2, seriestype="vline", v_lines_den, label="", color=:indigo, linestyle=:dash, linewidth=0.7)
    plot!(p2, title=("Phase $(latexstring(lat_name))"))
    # plot!(p2, seriestype="vline", v_line_pos, label="", color=:blue, linestyle=:dash, linewidth=0.7)
    # plot!(p2, seriestype="hline", [phase45], label="", color=:green, linestyle=:dash, linewidth=0.7)
    # ticks_pos = [10.0^p for p in p_b:p_e]
    # ticks_pos_labels = ["10^{$p}" for p in p_b:p_e]
    plot!(p2, xticks=(ticks_pos, ticks_pos_labels))
    ytisks2, ylabels2 = Plots.yticks(p2)[1]
    ylabels3 = [latexstring("$(y)°") for y in ylabels2]
    yticks!(ytisks2, ylabels3)
    # multicolor_xticks!(p2, :black, :red, v_line_pos, v_line_labels)
    p = plot(p1, p2, layout=(2, 1))
    savefig(p, "$name.png")
    savefig(p, "$name.svg")
    savefig(p, "$name.pdf")
    nothing
end
# test_value = 1 + 1e-4s
# append_to_file("test_value = $test_value")
# test_value_s_expanded = s_expand(test_value)
# first_order_s_presentation("test_value", test_value_s_expanded)

d_bebecece1 = 1
append_to_file("d_bebecece1 = $d_bebecece1")
d_bebecece1_v = substitute.(d_bebecece1, (com_values,))[1]
append_to_file("d_bebecece1_v = $d_bebecece1_v")

d_bece1 = g_m * r_π * r_o
append_to_file("d_bece1 = $d_bece1")
d_bece1_v = substitute.(d_bece1, (com_values,))[1]
append_to_file("d_bece1_v = $d_bece1_v")

d_bebe1 = r_o
append_to_file("d_bebe1 = $d_bebe1")
d_bebe1_v = substitute.(d_bebe1, (com_values,))[1]
append_to_file("d_bebe1_v = $d_bebe1_v")

d_cece1 = r_π
append_to_file("d_cece1 = $d_cece1")
d_cece1_v = substitute.(d_cece1, (com_values,))[1]
append_to_file("d_cece1_v = $d_cece1_v")

d_1 = r_π * r_o
append_to_file("d1 = $d_1")
d1_v = substitute.(d_1, (com_values,))[1]
append_to_file("d1_v = $d1_v")


# d_bebecece2 = d_bebecece1 + s * C_μ * (d_bece1 + d_bebe1 + d_cece1)
# append_to_file("d_bebece2 = $d_bebecece2")
# d_bebecece2_v = substitute.(d_bebecece2, (com_values,))[1]
# append_to_file("d_bebece2_v = $d_bebecece2_v")
# d_bebecece2_s = s_expand(d_bebecece2)
# append_to_file("d_bebecece2_s = $d_bebecece2_s")
# # d_bebecece2_v_s = s_expand(d_bebecece2_v)
# d_bebecece2_v_s = substitute.(d_bebecece2_s, (com_values,))
# append_to_file("d_bebecece2_v_s = $d_bebecece2_v_s")
# first_order_s_presentation("d_bebecece2", d_bebecece2_v_s, "d_{bebecece_2}")

# d_bece2 = d_bece1
# d_bebe2 = d_bebe1
# d_cece2 = d_cece1

# d_bcbc2 = s * C_μ * d_1
# append_to_file("d_bcbc2 = $d_bcbc2")
# d_bcbc2_v = substitute.(d_bcbc2, (com_values,))[1]
# append_to_file("d_bcbc2_v = $d_bcbc2_v")
# d_bcbc2_s = s_expand(d_bcbc2)
# append_to_file("d_bcbc2_s = $d_bcbc2_s")
# # d_bcbc2_v_s = s_expand(d_bcbc2_v)
# d_bcbc2_v_s = substitute.(d_bcbc2_s, (com_values,))
# append_to_file("d_bcbc2_v_s = $d_bcbc2_v_s")
# first_order_s_presentation("d_bcbc2", d_bcbc2_v_s, "d_{bcbc_2}")

# d_2 = d_1

d_bebecece2 = d_bebecece1
d_bece2 = d_bece1
d_bebe2 = d_bebe1
d_cece2 = d_cece1
d_bcbc2 = 0
d_2 = d_1

# d_bebecece3 = d_bebecece2 + s * C_π * (d_cece2 + d_bcbc2)
# append_to_file("d_bebecece3 = $d_bebecece3")
# d_bebecece3_v = substitute.(d_bebecece3, (com_values,))[1]
# append_to_file("d_bebecece3_v = $d_bebecece3_v")
# d_bebecece3_s = s_expand(d_bebecece3)
# append_to_file("d_bebecece3_s = $d_bebecece3_s")
# # d_bebecece3_v_s = s_expand(d_bebecece3_v)
# d_bebecece3_v_s = substitute.(d_bebecece3_s, (com_values,))
# append_to_file("d_bebecece3_v_s = $d_bebecece3_v_s")
# first_order_s_presentation("d_bebecece3", d_bebecece3_v_s, "d_{bebecece_3}")


# d_bece3 = d_bece2

# d_bebe3 = d_bebe2 + s * C_π * d_2
# append_to_file("d_bebe3 = $d_bebe3")
# d_bebe3_v = substitute.(d_bebe3, (com_values,))[1]
# append_to_file("d_bebe3_v = $d_bebe3_v")
# d_bebe3_s = s_expand(d_bebe3)
# append_to_file("d_bebe3_s = $d_bebe3_s")
# # d_bebe3_v_s = s_expand(d_bebe3_v)
# d_bebe3_v_s = substitute.(d_bebe3_s, (com_values,))
# append_to_file("d_bebe3_v_s = $d_bebe3_v_s")
# first_order_s_presentation("d_bebe3", d_bebe3_v_s, "d_{bebe_3}")

# d_cece3 = d_cece2
# d_bcbc3 = d_bcbc2
# d_3 = d_2

d_bebecece3 = d_bebecece2
d_bece3 = d_bece2
d_bebe3 = d_bebe2
d_cece3 = d_cece2
d_bcbc3 = d_bcbc2
d_3 = d_2


d_bebecece4 = d_bebecece3
d_bece4 = d_bece3
d_bebe4 = d_bebe3

# d_cece4 = d_cece3
d_cece4 = d_cece3 + r_bb * d_bebecece3
append_to_file("d_cece4 = $d_cece4")
d_cece4_v = substitute.(d_cece4, (com_values,))[1]
append_to_file("d_cece4_v = $d_cece4_v")
d_cece4_s = s_expand(d_cece4)
append_to_file("d_cece4_s = $d_cece4_s")
# d_cece4_v_s = s_expand(d_cece4_v)
d_cece4_v_s = substitute.(d_cece4_s, (com_values,))
append_to_file("d_cece4_v_s = $d_cece4_v_s")
first_order_s_presentation("d_cece4", d_cece4_v_s, "d_{cece_4}")


d_bcbc4 = d_bcbc3

# d_4 = d_3
d_4 = d_3 + r_bb * (d_bebe3 + d_bcbc3)
append_to_file("d_4 = $d_4")
d_4_v = substitute.(d_4, (com_values,))[1]
append_to_file("d4_v = $d_4_v")
d_4_s = s_expand(d_4)
append_to_file("d_4_s = $d_4_s")
# d_4_v_s = s_expand(d_4_v)
d_4_v_s = substitute.(d_4_s, (com_values,))
append_to_file("d_4_v_s = $d_4_v_s")
first_order_s_presentation("d_4", d_4_v_s, "d_4")



d_bebecece5 = d_bebecece4
d_bece5 = d_bece4
d_bebe5 = d_bebe4
d_cece5 = d_cece4


d_bcbc5 = d_bcbc4 + R_E * d_bebecece5
append_to_file("d_bcbc5 = $d_bcbc5")
d_bcbc5_v = substitute.(d_bcbc5, (com_values,))[1]
append_to_file("d_bcbc5_v = $d_bcbc5_v")
d_bcbc5_s = s_expand(d_bcbc5)
append_to_file("d_bcbc5_s = $d_bcbc5_s")
# d_bcbc5_v_s = s_expand(d_bcbc5_v)
d_bcbc5_v_s = substitute.(d_bcbc5_s, (com_values,))
append_to_file("d_bcbc5_v_s = $d_bcbc5_v_s")
first_order_s_presentation("d_bcbc5", d_bcbc5_v_s, "d_{bcbc_5}")

d_5 = d_4 + R_E * (d_bece4 + d_bebe4 + d_cece4)
append_to_file("d_5 = $d_5")
d_5_v = substitute.(d_5, (com_values,))[1]
append_to_file("d_5_v = $d_5_v")
d_5_s = s_expand(d_5)
append_to_file("d_5_s = $d_5_s")
# d_5_v_s = s_expand(d_5_v)
d_5_v_s = substitute.(d_5_s, (com_values,))
append_to_file("d_5_v_s = $d_5_v_s")
first_order_s_presentation("d_5", d_5_v_s, "d_5")



d_bbcc6 = d_bebecece5
d_bc6 = d_bece5
d_bb6 = d_bebe5
d_cc6 = d_cece5
d_bcbc6 = d_bcbc5
d_6 = d_5

# d_bbcc6 = (s * C_E2 * R_E2 + 1) * d_bebecece5
# append_to_file("d_bbcc6 = $d_bbcc6")
# d_bbcc6_v = substitute.(d_bbcc6, (com_values,))[1]
# append_to_file("d_bbcc6_v = $d_bbcc6_v")
# d_bbcc6_s = s_expand(d_bbcc6)
# append_to_file("d_bbcc6_s = $d_bbcc6_s")
# # d_bbcc6_v_s = s_expand(d_bbcc6_v)
# d_bbcc6_v_s = substitute.(d_bbcc6_s, (com_values,))
# append_to_file("d_bbcc6_v_s = $d_bbcc6_v_s")
# first_order_s_presentation("d_bbcc6", d_bbcc6_v_s, "d_{bbcc_6}")

# d_bc6 = (s * C_E2 * R_E2 + 1) * d_bece5
# append_to_file("d_bc6 = $d_bc6")
# d_bc6_v = substitute.(d_bc6, (com_values,))[1]
# append_to_file("d_bc6_v = $d_bc6_v")
# d_bc6_s = s_expand(d_bc6)
# append_to_file("d_bc6_s = $d_bc6_s")
# d_bc6_v_s = substitute.(d_bc6_s, (com_values,))
# # d_bc6_v_s = s_expand(d_bc6_v)
# append_to_file("d_bc6_v_s = $d_bc6_v_s")
# first_order_s_presentation("d_bc6", d_bc6_v_s, "d_{bc_6}")

# d_bb6 = (s * C_E2 * R_E2 + 1) * d_bebe5
# append_to_file("d_bb6 = $d_bb6")
# d_bb6_v = substitute.(d_bb6, (com_values,))[1]
# append_to_file("d_bb6_v = $d_bb6_v")
# d_bb6_s = s_expand(d_bb6)
# append_to_file("d_bb6_s = $d_bb6_s")
# # d_bb6_v_s = s_expand(d_bb6_v)
# d_bb6_v_s = substitute.(d_bb6_s, (com_values,))
# append_to_file("d_bb6_v_s = $d_bb6_v_s")
# first_order_s_presentation("d_bb6", d_bb6_v_s, "d_{bb_6}")

# d_cc6 = (s * C_E2 * R_E2 + 1) * d_cece5
# append_to_file("d_cc6 = $d_cc6")
# d_cc6_v = substitute.(d_cc6, (com_values,))[1]
# append_to_file("d_cc6_v = $d_cc6_v")
# d_cc6_s = s_expand(d_cc6)
# append_to_file("d_cc6_s = $d_cc6_s")
# # d_cc6_v_s = s_expand(d_cc6_v)
# d_cc6_v_s = substitute.(d_cc6_s, (com_values,))
# append_to_file("d_cc6_v_s = $d_cc6_v_s")
# first_order_s_presentation("d_cc6", d_cc6_v_s, "d_{cc_6}")

# d_bcbc6 = (s * C_E2 * R_E2 + 1) * d_bcbc5 + R_E2 * d_bebecece5
# append_to_file("d_bcbc6 = $d_bcbc6")
# d_bcbc6_v = substitute.(d_bcbc6, (com_values,))[1]
# append_to_file("d_bcbc6_v = $d_bcbc6_v")
# d_bcbc6_s = s_expand(d_bcbc6)
# append_to_file("d_bcbc6_s = $d_bcbc6_s")
# # d_bcbc6_v_s = s_expand(d_bcbc6_v)
# d_bcbc6_v_s = substitute.(d_bcbc6_s, (com_values,))
# append_to_file("d_bcbc6_v_s = $d_bcbc6_v_s")
# first_order_s_presentation("d_bcbc6", d_bcbc6_v_s, "d_{bcbc_6}")

# d_6 = (s * C_E2 * R_E2 + 1) * d_5 + R_E2 * (d_bece5 + d_bebe5 + d_cece5)
# append_to_file("d_6 = $d_6")
# d_6_v = substitute.(d_6, (com_values,))[1]
# append_to_file("d_6_v = $d_6_v")
# d_6_s = s_expand(d_6)
# append_to_file("d_6_s = $d_6_s")
# # d6_v_s = s_expand(d6_v)
# d_6_v_s = substitute.(d_6_s, (com_values,))
# append_to_file("d6_v_s = $d_6_v_s")
# first_order_s_presentation("d_6", d_6_v_s, "d_6")




d_bbcc7 = R_B * d_bbcc6 + d_cc6 + d_bcbc6
append_to_file("d_bbcc7 = $d_bbcc7")
d_bbcc7_v = substitute.(d_bbcc7, (com_values,))[1]
append_to_file("d_bbcc7_v = $d_bbcc7_v")
d_bbcc7_s = s_expand(d_bbcc7)
append_to_file("d_bbcc7_s = $d_bbcc7_s")
# d_bbcc7_v_s = s_expand(d_bbcc7_v)
d_bbcc7_v_s = substitute.(d_bbcc7_s, (com_values,))
append_to_file("d_bbcc7_v_s = $d_bbcc7_v_s")
first_order_s_presentation("d_bbcc7", d_bbcc7_v_s, "d_{bbcc_7}")

d_bc7 = R_B * d_bc6
append_to_file("d_bc7 = $d_bc7")
d_bc7_v = substitute.(d_bc7, (com_values,))[1]
append_to_file("d_bc7_v = $d_bc7_v")
d_bc7_s = s_expand(d_bc7)
append_to_file("d_bc7_s = $d_bbcc7_s")
# d_bbcc7_v_s = s_expand(d_bbcc7_v)
d_bc7_v_s = substitute.(d_bc7_s, (com_values,))
append_to_file("d_bc7_v_s = $d_bbcc7_v_s")
first_order_s_presentation("d_bc7", d_bc7_v_s, "d_{bc_7}")

d_bb7 = R_B * d_bb6 + d_6
append_to_file("d_bb7 = $d_bb7")
d_bb7_v = substitute.(d_bb7, (com_values,))[1]
append_to_file("d_bb7_v = $d_bb7_v")
d_bb7_s = s_expand(d_bb7)
append_to_file("d_bb7_s = $d_bb7_s")
# d_bb7_v_s = s_expand(d_bb7_v)
d_bb7_v_s = substitute.(d_bb7_s, (com_values,))
append_to_file("d_bb7_v_s = $d_bb7_v_s")
first_order_s_presentation("d_bb7", d_bb7_v_s, "d_{bb_7}")

d_cc7 = R_B * d_cc6
append_to_file("d_cc7 = $d_cc7")
d_cc7_v = substitute.(d_cc7, (com_values,))[1]
append_to_file("d_cc7_v = $d_cc7_v")
d_cc7_s = s_expand(d_cc7)
append_to_file("d_cc7_s = $d_cc7_s")
# d_cc7_v_s = s_expand(d_cc7_v)
d_cc7_v_s = substitute.(d_cc7_s, (com_values,))
append_to_file("d_cc7_v_s = $d_cc7_v_s")
first_order_s_presentation("d_cc7", d_cc7_v_s, "d_{cc_7}")

d_bcbc7 = R_B * d_bcbc6
append_to_file("d_bcbc7 = $d_bcbc7")
d_bcbc7_v = substitute.(d_bcbc7, (com_values,))[1]
append_to_file("d_bcbc7_v = $d_bcbc7_v")
d_bcbc7_s = s_expand(d_bcbc7)
append_to_file("d_bcbc7_s = $d_bcbc7_s")
# d_bcbc7_v_s = s_expand(d_bcbc7_v)
d_bcbc7_v_s = substitute.(d_bcbc7_s, (com_values,))
append_to_file("d_bcbc7_v_s = $d_bcbc7_v_s")
first_order_s_presentation("d_bcbc7", d_bcbc7_v_s, "d_{bcbc_7}")

d_7 = R_B * d_6
append_to_file("d_7 = $d_7")
d_7_v = substitute.(d_7, (com_values,))[1]
append_to_file("d_7_v = $d_7_v")
d_7_s = s_expand(d_7)
append_to_file("d_7_s = $d_7_s")
# d_7_v_s = s_expand(d_7_v)
d_7_v_s = substitute.(d_7_s, (com_values,))
append_to_file("d_7_v_s = $d_7_v_s")
first_order_s_presentation("d_7", d_7_v_s, "d_7")


d_bbcc8 = R_C * d_bbcc7 + d_bb7 + d_bcbc7
append_to_file("d_bbcc8 = $d_bbcc8")
d_bbcc8_v = substitute.(d_bbcc8, (com_values,))[1]
append_to_file("d_bbcc8_v = $d_bbcc8_v")
d_bbcc8_s = s_expand(d_bbcc8)
append_to_file("d_bbcc8_s = $d_bbcc8_s")
# d_bbcc8_v_s = s_expand(d_bbcc8_v)
d_bbcc8_v_s = substitute.(d_bbcc8_s, (com_values,))
append_to_file("d_bbcc8_v_s = $d_bbcc8_v_s")
first_order_s_presentation("d_bbcc8", d_bbcc8_v_s, "d_{bbcc_8}")

d_bc8 = R_C * d_bc7
append_to_file("d_bc8 = $d_bc8")
d_bc8_v = substitute.(d_bc8, (com_values,))[1]
append_to_file("d_bc8_v = $d_bc8_v")
d_bc8_s = s_expand(d_bc8)
append_to_file("d_bc8_s = $d_bc8_s")
# d_bc8_v_s = s_expand(d_bc8_v)
d_bc8_v_s = substitute.(d_bc8_s, (com_values,))
append_to_file("d_bc8_v_s = $d_bc8_v_s")
first_order_s_presentation("d_bc8", d_bc8_v_s, "d_{bc_8}")

d_bb8 = R_C * d_bb7
append_to_file("d_bb8 = $d_bb8")
d_bb8_v = substitute.(d_bb8, (com_values,))[1]
append_to_file("d_bb8_v = $d_bb8_v")
d_bb8_s = s_expand(d_bb8)
append_to_file("d_bb8_s = $d_bb8_s")
# d_bb8_v_s = s_expand(d_bb8_v)
d_bb8_v_s = substitute.(d_bb8_s, (com_values,))
append_to_file("d_bb8_v_s = $d_bb8_v_s")
first_order_s_presentation("d_bb8", d_bb8_v_s, "d_{bb_8}")


d_cc8 = R_C * d_cc7 + d_7
append_to_file("d_cc8 = $d_cc8")
d_cc8_v = substitute.(d_cc8, (com_values,))[1]
append_to_file("d_cc8_v = $d_cc8_v")
d_cc8_s = s_expand(d_cc8)
append_to_file("d_cc8_s = $d_cc8_s")
# d_cc8_v_s = s_expand(d_cc8_v)
d_cc8_v_s = substitute.(d_cc8_s, (com_values,))
append_to_file("d_cc8_v_s = $d_cc8_v_s")
first_order_s_presentation("d_cc8", d_cc8_v_s, "d_{cc_8}")

d_bcbc8 = R_C * d_bcbc7
append_to_file("d_bcbc8 = $d_bcbc8")
d_bcbc8_v = substitute.(d_bcbc8, (com_values,))[1]
append_to_file("d_bcbc8_v = $d_bcbc8_v")
d_bcbc8_s = s_expand(d_bcbc8)
append_to_file("d_bcbc8_s = $d_bcbc8_s")
# d_bcbc8_v_s = s_expand(d_bcbc8_v)
d_bcbc8_v_s = substitute.(d_bcbc8_s, (com_values,))
append_to_file("d_bcbc8_v_s = $d_bcbc8_v_s")
first_order_s_presentation("d_bcbc8", d_bcbc8_v_s, "d_{bcbc_8}")


d_8 = R_C * d_7
append_to_file("d_8 = $d_8")
d_8_v = substitute.(d_8, (com_values,))[1]
append_to_file("d_8_v = $d_8_v")
d_8_s = s_expand(d_8)
append_to_file("d_8_s = $d_8_s")
d_8_v_s = substitute.(d_8_s, (com_values,))
# d8_v_s = s_expand(d8_v)
append_to_file("d_8_v_s = $d_8_v_s")
first_order_s_presentation("d_8", d_8_v_s, "d_8")


# d_bbcc9 = s * C_1 * d_bbcc8
# d_bc9 = s * C_1 * d_bc8
# d_bb9 = s * C_1 * d_bb8
# d_cc9 = s * C_1 * d_cc8 + d_bbcc8
# d_bcbc9 = s * C_1 * d_bcbc8
# d_9 = s * C_1 * d_8 + d_bb8 + d_bcbc8

# d_bbcc9 = d_bbcc8
# d_bc9 = d_bc8
# d_bb9 = d_bb8
# d_cc9 = d_cc8
# d_bcbc9 = d_bcbc8
# d_9 = d_8


# d_bbcc10 = s * C_2 * d_bbcc9
# d_bc10 = s * C_2 * d_bc9
# d_bb10 = s * C_2 * d_bb9 + d_bbcc9
# d_cc10 = s * C_2 * d_cc9
# d_bcbc10 = s * C_2 * d_bcbc9
# d_10 = s * C_2 * d_9 + d_cc9 + d_bcbc9

# d_bbcc10 = d_bbcc9
# d_bc10 = d_bc9
# d_bb10 = d_bb9
# d_cc10 = d_cc9
# d_bcbc10 = d_bcbc9
# d_10 = d_9

d_bbcc9 = s * C_1 * d_bbcc8
append_to_file("d_bbcc9 = $d_bbcc9")
d_bbcc9_v = substitute.(d_bbcc9, (com_values,))[1]
append_to_file("d_bbcc9_v = $d_bbcc9_v")
d_bbcc9_s = s_expand(d_bbcc9)
append_to_file("d_bbcc9_s = $d_bbcc9_s")
d_bbcc9_v_s = substitute.(d_bbcc9_s, (com_values,))
append_to_file("d_bbcc9_v_s = $d_bbcc9_v_s")
first_order_s_presentation("d_bbcc9", d_bbcc9_v_s, "d_{bbcc_9}")

d_bc9 = s * C_1 * d_bc8
append_to_file("d_bc9 = $d_bc9")
d_bc9_v = substitute.(d_bc9, (com_values,))[1]
append_to_file("d_bc9_v = $d_bc9_v")
d_bc9_s = s_expand(d_bc9)
append_to_file("d_bc9_s = $d_bc9_s")
d_bc9_v_s = substitute.(d_bc9_s, (com_values,))
append_to_file("d_bc9_v_s = $d_bc9_v_s")
first_order_s_presentation("d_bc9", d_bc9_v_s, "d_{bc_9}")

d_bb9 = s * C_1 * d_bb8
append_to_file("d_bb9 = $d_bb9")
d_bb9_v = substitute.(d_bb9, (com_values,))[1]
append_to_file("d_bb9_v = $d_bb9_v")
d_bb9_s = s_expand(d_bb9)
append_to_file("d_bb9_s = $d_bb9_s")
d_bb9_v_s = substitute.(d_bb9_s, (com_values,))
append_to_file("d_bb9_v_s = $d_bb9_v_s")
first_order_s_presentation("d_bb9", d_bb9_v_s, "d_{bb_9}")

d_cc9 = s * C_1 * d_cc8 + d_bbcc8
append_to_file("d_cc9 = $d_cc9")
d_cc9_v = substitute.(d_cc9, (com_values,))[1]
append_to_file("d_cc9_v = $d_cc9_v")
d_cc9_s = s_expand(d_cc9)
append_to_file("d_cc9_s = $d_cc9_s")
d_cc9_v_s = substitute.(d_cc9_s, (com_values,))
append_to_file("d_cc9_v_s = $d_cc9_v_s")
first_order_s_presentation("d_cc9", d_cc9_v_s, "d_{cc_9}")

d_bcbc9 = s * C_1 * d_bcbc8
append_to_file("d_bcbc9 = $d_bcbc9")
d_bcbc9_v = substitute.(d_bcbc9, (com_values,))[1]
append_to_file("d_bcbc9_v = $d_bcbc9_v")
d_bcbc9_s = s_expand(d_bcbc9)
append_to_file("d_bcbc9_s = $d_bcbc9_s")
d_bcbc9_v_s = substitute.(d_bcbc9_s, (com_values,))
append_to_file("d_bcbc9_v_s = $d_bcbc9_v_s")
first_order_s_presentation("d_bcbc9", d_bcbc9_v_s, "d_{bcbc_9}")

d_9 = s * C_1 * d_8 + d_bb8 + d_bcbc8
append_to_file("d_9 = $d_9")
d_9_v = substitute.(d_9, (com_values,))[1]
append_to_file("d_9_v = $d_9_v")
d_9_s = s_expand(d_9)
append_to_file("d_9_s = $d_9_s")
d_9_v_s = substitute.(d_9_s, (com_values,))
append_to_file("d_9_v_s = $d_9_v_s")
first_order_s_presentation("d_9", d_9_v_s, "d_9")

d_bbcc10 = d_bbcc9
d_bc10 = d_bc9
d_bb10 = d_bb9
d_cc10 = d_cc9
d_bcbc10 = d_bcbc9
d_10 = d_9


# d_bbcc10 = s * C_2 * d_bbcc9
# append_to_file("d_bbcc10 = $d_bbcc10")
# d_bbcc10_v = substitute.(d_bbcc10, (com_values,))[1]
# append_to_file("d_bbcc10_v = $d_bbcc10_v")
# d_bbcc10_s = s_expand(d_bbcc10)
# append_to_file("d_bbcc10_s = $d_bbcc10_s")
# d_bbcc10_v_s = substitute.(d_bbcc10_s, (com_values,))
# append_to_file("d_bbcc10_v_s = $d_bbcc10_v_s")
# first_order_s_presentation("d_bbcc10", d_bbcc10_v_s, "d_{bbcc_10}")


# d_bc10 = s * C_2 * d_bc9
# append_to_file("d_bc10 = $d_bc10")
# d_bc10_v = substitute.(d_bc10, (com_values,))[1]
# append_to_file("d_bc10_v = $d_bc10_v")
# d_bc10_s = s_expand(d_bc10)
# append_to_file("d_bc10_s = $d_bc10_s")
# d_bc10_v_s = substitute.(d_bc10_s, (com_values,))
# append_to_file("d_bc10_v_s = $d_bc10_v_s")
# first_order_s_presentation("d_bc10", d_bc10_v_s, "d_{bc_10}")

# d_bb10 = s * C_2 * d_bb9 + d_bbcc9
# append_to_file("d_bb10 = $d_bb10")
# d_bb10_v = substitute.(d_bb10, (com_values,))[1]
# append_to_file("d_bb10_v = $d_bb10_v")
# d_bb10_s = s_expand(d_bb10)
# append_to_file("d_bb10_s = $d_bb10_s")
# d_bb10_v_s = substitute.(d_bb10_s, (com_values,))
# append_to_file("d_bb10_v_s = $d_bb10_v_s")
# first_order_s_presentation("d_bb10", d_bb10_v_s, "d_{bb_10}")

# d_cc10 = s * C_2 * d_cc9
# append_to_file("d_cc10 = $d_cc10")
# d_cc10_v = substitute.(d_cc10, (com_values,))[1]
# append_to_file("d_cc10_v = $d_cc10_v")
# d_cc10_s = s_expand(d_cc10)
# append_to_file("d_cc10_s = $d_cc10_s")
# d_cc10_v_s = substitute.(d_cc10_s, (com_values,))
# append_to_file("d_cc10_v_s = $d_cc10_v_s")
# first_order_s_presentation("d_cc10", d_cc10_v_s, "d_{cc_10}")

# d_bcbc10 = s * C_2 * d_bcbc9
# append_to_file("d_bcbc10 = $d_bcbc10")
# d_bcbc10_v = substitute.(d_bcbc10, (com_values,))[1]
# append_to_file("d_bcbc10_v = $d_bcbc10_v")
# d_bcbc10_s = s_expand(d_bcbc10)
# append_to_file("d_bcbc10_s = $d_bcbc10_s")
# d_bcbc10_v_s = substitute.(d_bcbc10_s, (com_values,))
# append_to_file("d_bcbc10_v_s = $d_bcbc10_v_s")
# first_order_s_presentation("d_bcbc10", d_bcbc10_v_s, "d_{bcbc_10}")

# d_10 = s * C_2 * d_9 + d_cc9 + d_bcbc9
# append_to_file("d_10 = $d_10")
# d_10_v = substitute.(d_10, (com_values,))[1]
# append_to_file("d_10_v = $d_10_v")
# d_10_s = s_expand(d_10)
# append_to_file("d_10_s = $d_10_s")
# d_10_v_s = substitute.(d_10_s, (com_values,))
# append_to_file("d_10_v_s = $d_10_v_s")
# first_order_s_presentation("d_10", d_10_v_s, "d_{10}")

d_bbcc11 = R_L * d_bbcc10 + d_bb10 + d_bcbc10
append_to_file("d_bbcc11 = $d_bbcc11")
d_bbcc11_v = substitute.(d_bbcc11, (com_values,))[1]
append_to_file("d_bbcc11_v = $d_bbcc11_v")
d_bbcc11_s = s_expand(d_bbcc11)
append_to_file("d_bbcc11_s = $d_bbcc11_s")
d_bbcc11_v_s = substitute.(d_bbcc11_s, (com_values,))
append_to_file("d_bbcc11_v_s = $d_bbcc11_v_s")
first_order_s_presentation("d_bbcc11", d_bbcc11_v_s, "d_{bbcc_{11}}")

d_bc11 = R_L * d_bc10
append_to_file("d_bc11 = $d_bc11")
d_bc11_v = substitute.(d_bc11, (com_values,))[1]
append_to_file("d_bc11_v = $d_bc11_v")
d_bc11_s = s_expand(d_bc11)
append_to_file("d_bc11_s = $d_bc11_s")
d_bc11_v_s = substitute.(d_bc11_s, (com_values,))
append_to_file("d_bc11_v_s = $d_bc11_v_s")
first_order_s_presentation("d_bc11", d_bc11_v_s, "d_{bc_{11}}")

d_bb11 = R_L * d_bb10
append_to_file("d_bb11 = $d_bb11")
d_bb11_v = substitute.(d_bb11, (com_values,))[1]
append_to_file("d_bb11_v = $d_bb11_v")
d_bb11_s = s_expand(d_bb11)
append_to_file("d_bb11_s = $d_bb11_s")
d_bb11_v_s = substitute.(d_bb11_s, (com_values,))
append_to_file("d_bb11_v_s = $d_bb11_v_s")
first_order_s_presentation("d_bb11", d_bb11_v_s, "d_{bb_{11}}")

d_cc11 = R_L * d_cc10 + d_10
append_to_file("d_cc11 = $d_cc11")
d_cc11_v = substitute.(d_cc11, (com_values,))[1]
append_to_file("d_cc11_v = $d_cc11_v")
d_cc11_s = s_expand(d_cc11)
append_to_file("d_cc11_s = $d_cc11_s")
d_cc11_v_s = substitute.(d_cc11_s, (com_values,))
append_to_file("d_cc11_v_s = $d_cc11_v_s")
first_order_s_presentation("d_cc11", d_cc11_v_s, "d_{cc_{11}}")

d_bcbc11 = R_L * d_bcbc10
append_to_file("d_bcbc11 = $d_bcbc11")
d_bcbc11_v = substitute.(d_bcbc11, (com_values,))[1]
append_to_file("d_bcbc11_v = $d_bcbc11_v")
d_bcbc11_s = s_expand(d_bcbc11)
append_to_file("d_bcbc11_s = $d_bcbc11_s")
d_bcbc11_v_s = substitute.(d_bcbc11_s, (com_values,))
append_to_file("d_bcbc11_v_s = $d_bcbc11_v_s")
first_order_s_presentation("d_bcbc11", d_bcbc11_v_s, "d_{bcbc_{11}}")

d_11 = R_L * d_10
append_to_file("d_11 = $d_11")
d_11_v = substitute.(d_11, (com_values,))[1]
append_to_file("d_11_v = $d_11_v")
d_11_s = s_expand(d_11)
append_to_file("d_11_s = $d_11_s")
d_11_v_s = substitute.(d_11_s, (com_values,))
append_to_file("d_11_v_s = $d_11_v_s")
first_order_s_presentation("d_11", d_11_v_s, "d_{11}")


d_bbcc12 = d_bbcc11
d_bc12 = d_bc11
d_bb12 = d_bb11

d_cc12 = d_cc11 + R_S * d_bbcc11
append_to_file("d_cc12 = $d_cc12")
d_cc12_v = substitute.(d_cc12, (com_values,))[1]
append_to_file("d_cc12_v = $d_cc12_v")
d_cc12_s = s_expand(d_cc12)
append_to_file("d_cc12_s = $d_cc12_s")
d_cc12_v_s = substitute.(d_cc12_s, (com_values,))
append_to_file("d_cc12_v_s = $d_cc12_v_s")
first_order_s_presentation("d_cc12", d_cc12_v_s, "d_{cc_{12}}")

d_bcbc12 = d_bcbc11

d_12 = d_11 + R_S * (d_bb11 + d_bcbc11)
append_to_file("d_12 = $d_12")
d_12_v = substitute.(d_12, (com_values,))[1]
append_to_file("d_12_v = $d_12_v")
d_12_s = s_expand(d_12)
append_to_file("d_12_s = $d_12_s")
d_12_v_s = substitute.(d_12_s, (com_values,))
append_to_file("d_12_v_s = $d_12_v_s")
first_order_s_presentation("d_12", d_12_v_s, "d_{12}")

A1 = d_bcbc12
append_to_file("A1 = $A1")
A1_v = substitute.(A1, (com_values,))[1]
append_to_file("A1_v = $A1_v")
A1_s = s_expand(A1)
append_to_file("A1_s = $A1_s")
A1_v_s = substitute.(A1_s, (com_values,))
append_to_file("A1_v_s = $A1_v_s")
a1start, a1stop, a1_v_lines = first_order_s_presentation("A1", A1_v_s, "A_{1}")

A2 = d_cc12 + d_bcbc12
append_to_file("A2 = $A2")
A2_v = substitute.(A2, (com_values,))[1]
append_to_file("A2_v = $A2_v")
A2_s = s_expand(A2)
append_to_file("A2_s = $A2_s")
A2_v_s = substitute.(A2_s, (com_values,))
append_to_file("A2_v_s = $A2_v_s")
a2start, a2stop, a2_v_lines = first_order_s_presentation("A2", A2_v_s, "A_{2}")

A3 = -d_bc12 + d_bcbc12
append_to_file("A3 = $A3")
A3_v = substitute.(A3, (com_values,))[1]
append_to_file("A3_v = $A3_v")
A3_s = s_expand(A3)
append_to_file("A3_s = $A3_s")
A3_v_s = substitute.(A3_s, (com_values,))
append_to_file("A3_v_s = $A3_v_s")
a3start, a3stop, a3_v_lines = first_order_s_presentation("A3", A3_v_s, "A_{3}")

A4 = d_bb12 + d_bcbc12
append_to_file("A4 = $A4")
A4_v = substitute.(A4, (com_values,))[1]
append_to_file("A4_v = $A4_v")
A4_s = s_expand(A4)
append_to_file("A4_s = $A4_s")
A4_v_s = substitute.(A4_s, (com_values,))
append_to_file("A4_v_s = $A4_v_s")
a4start, a4stop, a4_v_lines = first_order_s_presentation("A4", A4_v_s, "A_{4}")

A5 = d_12
append_to_file("A5 = $A5")
A5_v = substitute.(A5, (com_values,))[1]
append_to_file("A5_v = $A5_v")
A5_s = s_expand(A5)
append_to_file("A5_s = $A5_s")
A5_v_s = substitute.(A5_s, (com_values,))
append_to_file("A5_v_s = $A5_v_s")
a5start, a5stop, a5_v_lines = first_order_s_presentation("A5", A5_v_s, "A_{5}")

K_u = A3 / A2
append_to_file("K_u = $K_u")
K_u_v = substitute.(K_u, (com_values,))[1]
append_to_file("K_u_v = $K_u_v")
# append_to_file("K_u_s = $(A3_s / A2_s)")
# append_to_file("K_u_v_s = $(A3_v_s / A2_v_s)")
division_presentation("K_u", "K_u", A3_v_s, A2_v_s,
    a3_v_lines, a2_v_lines, [min(a3start, a2start), max(a3stop, a2stop)], dbels=true)

M = A3 / A1
append_to_file("M = $M")
M_v = substitute.(M, (com_values,))[1]
append_to_file("M_v = $M_v")
division_presentation("M", "M", A3_v_s, A1_v_s, a3_v_lines, a1_v_lines, [min(a3start, a1start), max(a3stop, a1stop)])

N = A3 / A5
append_to_file("N = $N")
N_v = substitute.(N, (com_values,))[1]
append_to_file("N_v = $N_v")
division_presentation("N", "N", A3_v_s, A5_v_s,
    a3_v_lines, a5_v_lines, [min(a3start, a5start), max(a3stop, a5stop)])

K_i = A3 / A4
append_to_file("K_i = $K_i")
K_i_v = substitute.(K_i, (com_values,))[1]
append_to_file("K_i_v = $K_i_v")
division_presentation("K_i", "K_i", A3_v_s, A4_v_s,
    a3_v_lines, a4_v_lines, [min(a3start, a4start), max(a3stop, a4stop)], dbels=true)

Z_I = A2 / A1
append_to_file("Z_I = $Z_I")
Z_I_v = substitute.(Z_I, (com_values,))[1]
append_to_file("Z_I_v = $Z_I_v")
division_presentation("Z_I", "Z_I", A2_v_s, A1_v_s,
    a2_v_lines, a1_v_lines, [min(a2start, a1start), max(a2stop, a1stop)])

Z_O = A5 / A2
append_to_file("Z_O = $Z_O")
Z_O_v = substitute.(Z_O, (com_values,))[1]
append_to_file("Z_O_v = $Z_O_v")
division_presentation("Z_O", "Z_O", A5_v_s, A2_v_s,
    a5_v_lines, a2_v_lines, [min(a5start, a2start), max(a5stop, a2stop)])
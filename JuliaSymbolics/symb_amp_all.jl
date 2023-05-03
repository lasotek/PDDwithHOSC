using Symbolics
using SymbolicUtils

SymbolicUtils.show_simplified[] = true

# @syms x::Real y::Real z::Complex f(::Number)::Real 

# Base.Complex{T}(r::Number, i::Complex) where {T<:Real} = r + im * i
# Base.Complex{T}(r::Complex, i::Number) where {T<:Real} = r + im * i
# Base.Complex{T}(r::Complex, i::Complex) where {T<:Real} = r + im * i

@variables g_m r_π r_bb r_o C_π C_μ R_E R_E2 R_B1 R_B2 R_B R_c R_L R_C f C_E C_1 C_2 s

com_values = Dict(
    R_E => 280,
    R_E2 => 280,
    R_B1 => 150e3,
    R_B2 => 56e3,
    R_B => 150e3 * 56e3 / (150e3 + 56e3),
    R_c => 1.3e3,
    R_L => 3.3e3,
    R_C => 1.3e3 * 3.3e3 / (1.3e3 + 3.3e3),
    g_m => 1.94e-01,
    r_π => 3.51e+03,
    r_bb => 3.30,
    r_o => 1.49e+04,
    C_π => 1.20e-10,
    C_μ => 2.40e-12,
    C_E => 750e-6,
    C_1 => 1e-6,
    C_2 => 1e-6,
    # s => 2π * 1e3 * im,
    # f => 1e3,
)


function s_expand(symbolic_poly_s, limit_power::Int64=100)
    res = Dict()
    const_v = simplify.(substitute.(symbolic_poly_s, (Dict(s => 0),)))
    res[0] = const_v[1]
    s_power::Int64 = 1
    while s_power <= limit_power
        symbolic_poly_s = simplify.(expand.(symbolic_poly_s - const_v[1]) / s)
        if iszero(symbolic_poly_s)
            break
        end
        const_v = simplify.(substitute.(symbolic_poly_s, (Dict(s => 0),)))
        res[s_power] = const_v[1]
        s_power += 1
    end
    return res

end

function sym_to_complex_f(symbololic_exp)

    function res_fun(f::Number)
        s_expandes = s_expand(symbololic_exp)
        @show s_expandes
        real_part = 0
        imag_part = 0
        for val in s_expandes
            power = val[1]
            con = val[2]
            @show power
            @show con
            if power % 2 == 0
                real_part += (-1)^(power / 2) * con * f^power * (2π)^power
            else
                imag_part += (-1)^((power - 1) / 2) * con * f^power * (2π)^power
            end
        end
        return real_part + im * imag_part
    end
    return res_fun
end

d_bebecece_1 = 1
d_cebe_1 = g_m * r_π * r_o
d_bebe_1 = r_o
d_cece_1 = r_π
d_1 = r_π * r_o

d_bebecece_2 = d_bebecece_1 + s * C_μ * (d_cebe_1 + d_bebe_1 + d_cece_1)
d_bece_2 = d_cebe_1
d_bebe_2 = d_bebe_1
d_cece_2 = d_cece_1
d_bcbc_2 = s * C_μ * d_1
d_2 = d_1

test = substitute.(d_becebece_2, (com_values,))[1]
@show test
test1 = s_expand(d_becebece_2)
@show test1
test_d = sym_to_complex_f(d_becebece_2)
test_d(10)
aa = substitute.(test_d, (Dict(f => 1e3),))[1]

test2 = s_expand(test)
test_d = sym_to_complex_f(test)
test_d(10)

d_bebecece_3 = d_bebecece_2 + s * C_π * (d_cece_2 + d_bcbc_2)
d_bece_3 = d_bece_2
d_bebe_3 = d_bebe_2 + s * C_π * d_2
d_cece_3 = d_cece_2
d_bcbc_3 = d_bcbc_2
d_3 = d_2

test = substitute.(d_becebece_3, (com_values,))[1]

res = s_expand(d_becebece_3)
r0 = substitute.(res[0], (com_values,))[1]
r1 = substitute.(res[1], (com_values,))[1]
r2 = substitute.(res[2], (com_values,))[1]
res = s_expand(d_bebe_3)
res = s_expand(d_bcbc_3)


d_bebecece_4 = d_bebecece_3
d_bece_4 = d_bece_3
d_bebe_4 = d_bebe_3
d_cece_4 = d_cece_3 + r_bb * d_bebecece_3
d_bcbc_4 = d_bcbc_3
d_4 = d_3 + r_bb * (d_bebe_3 + d_bcbc_3)

test = substitute.(d_becebece_4, (com_values,))[1]


d_bebecece_5 = d_bebecece_4
d_bece_5 = d_bece_4
d_bebe_5 = d_bebe_4
d_cece_5 = d_cece_4
d_bcbc_5 = d_bcbc_4 + R_E * d_bebecece_4
d_5 = d_4 + R_E * (d_bece_4 + d_bebe_4 + d_cece_4)

test = substitute.(d_becebece_5, (com_values,))[1]
# d_bcbc_6 = d_becebece_5
# d_bc_6 = d_bece_5
# d_bb_6 = d_bebe_5
# d_cc_6 = d_cece_5
# d_bcbc_6 = d_bcbc_5
# d_6 = d_5


d_bbcc_6 = (s * C_E * R_E2 + 1) * d_bebecece_5
d_bc_6 = (s * C_E * R_E2 + 1) * d_bece_5
d_bb_6 = (s * C_E * R_E2 + 1) * d_bebe_5
d_cc_6 = (s * C_E * R_E2 + 1) * d_cece_5
d_bcbc_6 = (s * C_E * R_E2 + 1) * d_bcbc_5 + R_E2 * d_bebecece_5
d_6 = (s * C_E * R_E2 + 1) * d_5 + R_E2 * (d_bece_5 + d_bebe_5 + d_cece_5)

test = substitute.(d_bcbc_6, (com_values,))[1]

d_bbcc_7 = R_B * d_bbcc_6 + d_cc_6 + d_bcbc_6
d_bc_7 = R_B * d_bc_6
d_bb_7 = R_B * d_bb_6 + d_6
d_cc_7 = R_B * d_cc_6
d_bcbc_7 = R_B * d_bcbc_6
d_7 = d_6

test = substitute.(d_bcbc_7, (com_values,))[1]

d_bbcc_8 = R_C * d_bbcc_7 + d_bb_7 + d_bc_7
d_bc_8 = R_C * d_bc_7
d_bb_8 = R_C * d_bb_7
d_cc_8 = R_C * d_cc_7
d_bcbc_8 = R_C * d_bcbc_7
d_8 = R_C * d_7

test = substitute.(d_bcbc_8, (com_values,))[1]

d_bbcc_9 = s * C_1 * d_bbcc_8
d_bc_9 = s * C_1 * d_bc_8
d_bb_9 = s * C_1 * d_bb_8
d_cc_9 = s * C_1 * d_cc_8 + d_bbcc_8
d_bcbc_9 = s * C_1 * d_bcbc_8
d_9 = s * C_1 * d_8 + d_bb_8 + d_bcbc_8

test = substitute.(d_bcbc_9, (com_values,))[1]

d_bbcc_10 = s * C_2 * d_bbcc_9
d_bc_10 = s * C_2 * d_bc_9
d_bb_10 = s * C_2 * d_bb_9 + d_bbcc_9
d_cc_10 = s * C_2 * d_cc_9
d_bcbc_10 = s * C_2 * d_bcbc_9
d_10 = s * C_2 * d_9 + d_cc_9 + d_bcbc_9

test = substitute.(d_bcbc_10, (com_values,))[1]

d_bbcc_11 = R_L * d_bbcc_10 + d_bb_10 + d_bcbc_10
d_bc_11 = R_L * d_bc_10
d_bb_11 = R_L * d_bb_10
d_cc_11 = R_L * d_cc_10 + d_10
d_bcbc_11 = R_L * d_bcbc_10
d_11 = R_L * d_10

test = substitute.(d_bcbc_11, (com_values,))[1]

A1 = d_bbcc_11
A2 = d_cc_11 + d_bcbc_11
A3 = -d_bc_11 + d_bcbc_11
A4 = d_bb_11 + d_bcbc_11
A5 = d_11

Ku = A3 / A2
KU_s = substitute.(Ku, (com_values,))[1]
test = substitute.(A3, (com_values,))[1]
# import Base: ^
test2(s) = substitute.(A3, (com_values,))[1]
test2(3)
# function ^(z::Complex{T}, n::Num) where {T<:Real}
#     r, θ = abs(z), angle(z)
#     w = r^n * exp(θ * n * im)
#     return real(w) + imag(w) * im
# end

test_s = s_expand(test)
@show(test_s)

testim = substitute.(test, (Dict(s => 2π * f * im),))[1]


(1 + im)^3
# Primitive MIMO example

![Circuit](3-RC-M-2.svg)

The circuit have 2 inputs (![V_I](https://render.githubusercontent.com/render/math?math=V_I) and ![I_I](https://render.githubusercontent.com/render/math?math=I_I)) and 2 outputs (![V_O](https://render.githubusercontent.com/render/math?math=V_O) and ![I_O](https://render.githubusercontent.com/render/math?math=I_O)).

The common denominator is (voltage input, current output)

(1) <img src="https://render.githubusercontent.com/render/math?math=D%3D%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(1%2B0)(4%2B5)%7D">


The numerators: Copy denominator, remove pair of input gates and output gate (if they exist in copy). Finally add a pair in a form: ![\phantom{ }^{(a_I+b_I)}_{(a_O+b_O)}](https://render.githubusercontent.com/render/math?math=%5Cphantom%7B%20%7D%5E%7B(a_I%2Bb_I)%7D_%7B(a_O%2Bb_O)%7D).
<!-- $D=\varDelta^{(1+0)(4+5)}_{(1+0)(4+5)}$ -->

(2) <img src="https://render.githubusercontent.com/render/math?math=N_%7Bvo%5Cleftarrow%20vi%7D%3D%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(3%2B5)(4%2B5)%7D">

<!-- $N_{vo\leftarrow vi}=\varDelta^{(1+0)(4+5)}_{(3+5)(4+5)}$ -->

(3) <img src="https://render.githubusercontent.com/render/math?math=N_%7Bvo%5Cleftarrow%20ii%7D%3D%5CvarDelta%5E%7B(1%2B0)(4%2B5)(2%2B0)%7D_%7B(1%2B0)(4%2B5)(3%2B5)%7D">

<!-- $N_{vo\leftarrow ii}=\varDelta^{(1+0)(4+5)(2+0)}_{(1+0)(4+5)(3+5)}$ -->

(4) <img src="https://render.githubusercontent.com/render/math?math=N_%7Bio%5Cleftarrow%20vi%7D%3D%5CvarDelta%5E%7B(1%2B0)%7D_%7B(4%2B5)%7D">

<!-- $N_{io\leftarrow vi}=\varDelta^{(1+0)}_{(4+5)}$ -->

(5) <img src="https://render.githubusercontent.com/render/math?math=N_%7Bio%5Cleftarrow%20ii%7D%3D%5CvarDelta%5E%7B(1%2B0)(2%2B0)%7D_%7B(1%2B0)(4%2B5)%7D">

<!-- $N_{io\leftarrow ii}=\varDelta^{(1+0)(2+0)}_{(1+0)(4+5)}$ -->

(6) <img src="https://render.githubusercontent.com/render/math?math=K_%7Bvo%5Cleftarrow%20vi%7D%3D%7B%7B%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(3%2B5)(4%2B5)%7D%7D%5Cover%7B%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(1%2B0)(4%2B5)%7D%7D%7D%3D%5Cleft(A%5Ccdot%20R_i%5Cright)%7B%7B-1%7D%5Cover%7Bs%5Ccdot%5Cleft(R_f%2BR_s%20%5Cright)%5Ccdot%5Cleft(%5Cleft(A%2B1%5Cright)%5Ccdot%20C_f%2BC_i%20%5Cright)%5Ccdot%20R_i%2B%5Cleft(R_f%2BR_s%2BR_i%20%5Cright)%7D%7D">

<!-- $K_{vo\leftarrow vi}={{\varDelta^{(1+0)(4+5)}_{(3+5)(4+5)}}\over{\varDelta^{(1+0)(4+5)}_{(1+0)(4+5)}}}=\left(A\cdot R_i\right){{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}$ -->

(7) <img src="https://render.githubusercontent.com/render/math?math=M_%7Bvo%5Cleftarrow%20ii%7D%3D%7B%7B%5CvarDelta%5E%7B(1%2B0)(4%2B5)(2%2B0)%7D_%7B(1%2B0)(4%2B5)(3%2B5)%7D%7D%5Cover%7B%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(1%2B0)(4%2B5)%7D%7D%7D%3D%5Cleft(A%5Ccdot%20R_s%5Ccdot%20R_i%20%5Cright)%7B%7B-1%7D%5Cover%7Bs%5Ccdot%5Cleft(R_f%2BR_s%20%5Cright)%5Ccdot%5Cleft(%5Cleft(A%2B1%5Cright)%5Ccdot%20C_f%2BC_i%20%5Cright)%5Ccdot%20R_i%2B%5Cleft(R_f%2BR_s%2BR_i%20%5Cright)%7D%7D">

<!-- $M_{vo\leftarrow ii}={{\varDelta^{(1+0)(4+5)(2+0)}_{(1+0)(4+5)(3+5)}}\over{\varDelta^{(1+0)(4+5)}_{(1+0)(4+5)}}}=\left(A\cdot R_s\cdot R_i \right){{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}$ -->

(8) <img src="https://render.githubusercontent.com/render/math?math=N_%7Bio%5Cleftarrow%20vi%7D%3D%7B%7B%5CvarDelta%5E%7B(1%2B0)%7D_%7B(4%2B5)%7D%7D%5Cover%7B%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(1%2B0)(4%2B5)%7D%7D%7D%3D%7B%7BA%5Ccdot%20R_i%7D%5Cover%7BR_o%7D%7D%7B%7B-1%7D%5Cover%7Bs%5Ccdot%5Cleft(R_f%2BR_s%20%5Cright)%5Ccdot%5Cleft(%5Cleft(A%2B1%5Cright)%5Ccdot%20C_f%2BC_i%20%5Cright)%5Ccdot%20R_i%2B%5Cleft(R_f%2BR_s%2BR_i%20%5Cright)%7D%7D">

<!-- $N_{io\leftarrow vi}={{\varDelta^{(1+0)}_{(4+5)}}\over{\varDelta^{(1+0)(4+5)}_{(1+0)(4+5)}}}={{A\cdot R_i}\over{R_o}}{{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}$ -->

(9) <img src="https://render.githubusercontent.com/render/math?math=K_%7Bio%5Cleftarrow%20ii%7D%3D%7B%7B%5CvarDelta%5E%7B(1%2B0)(2%2B0)%7D_%7B(1%2B0)(4%2B5)%7D%7D%5Cover%7B%5CvarDelta%5E%7B(1%2B0)(4%2B5)%7D_%7B(1%2B0)(4%2B5)%7D%7D%7D%3D%7B%7BA%5Ccdot%20R_s%5Ccdot%20R_i%7D%5Cover%7BR_o%7D%7D%7B%7B-1%7D%5Cover%7Bs%5Ccdot%5Cleft(R_f%2BR_s%20%5Cright)%5Ccdot%5Cleft(%5Cleft(A%2B1%5Cright)%5Ccdot%20C_f%2BC_i%20%5Cright)%5Ccdot%20R_i%2B%5Cleft(R_f%2BR_s%2BR_i%20%5Cright)%7D%7D">

<!-- $K_{io\leftarrow ii}={{\varDelta^{(1+0)(2+0)}_{(1+0)(4+5)}}\over{\varDelta^{(1+0)(4+5)}_{(1+0)(4+5)}}}={{A\cdot R_s\cdot R_i}\over{R_o}}{{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}$ -->
However, the calculation can be more effective if we represent the above formula as SoE:\
<img src="https://render.githubusercontent.com/render/math?math=X10%3DR_f%2BR_s%2BR_i"><!-- $X10=R_f+R_s+R_i$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X11%3DX10%5Ccdot%20R_o"><!-- $X11=X10\cdot R_o$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X12%3DA%2B1"><!-- $X12=A+1$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X13%3DX12%5Ccdot%20C_f"><!-- $X13=X12\cdot C_f$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X14%3DX13%2BC_i"><!-- $X14=X13+C_i$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X15%3DR_f%2BR_s"><!-- $X15=R_f+R_s$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X16%3DX15%5Ccdot%20X14%5Ccdot%20R_i%5Ccdot%20R_o"><!-- $X16=X15\cdot X14\cdot R_i\cdot R_o$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X17%3DA%5Ccdot%20R_i%5Ccdot%20R_o"><!-- $X17=A\cdot R_i\cdot R_o$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X18%3DA%5Ccdot%20R_s%5Ccdot%20R_i%5Ccdot%20R_o"><!-- $X18=A\cdot R_s\cdot R_i\cdot R_o$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X19%3DA%5Ccdot%20R_i"><!-- $X19=A\cdot R_i$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=X20%3DA%5Ccdot%20R_s%5Ccdot%20R_i"><!-- $X20=A\cdot R_s\cdot R_i$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=%5CvarDelta_%7B(1%2B0)(4%2B5)%7D%5E%7B(1%2B0)(4%2B5)%7D%3Ds%5Ccdot%20X16%2BX11"><!-- $\varDelta_{(1+0)(4+5)}^{(1+0)(4+5)}=s\cdot X16+X11$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=%5CvarDelta_%7B(3%2B5)(4%2B5)%7D%5E%7B(1%2B0)(4%2B5)%7D%20%3D-%20X17"><!-- $\varDelta_{(3+5)(4+5)}^{(1+0)(4+5)} =- X17$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=%5CvarDelta_%7B(3%2B5)(1%2B0)(4%2B5)%7D%5E%7B(2%2B0)(1%2B0)(4%2B5)%7D%20%3D-%20X18"><!-- $\varDelta_{(3+5)(1+0)(4+5)}^{(2+0)(1+0)(4+5)} =- X18$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=%5CvarDelta_%7B(4%2B5)%7D%5E%7B(1%2B0)%7D%3D-%20X19"><!-- $\varDelta_{(4+5)}^{(1+0)}=- X19$\ -->\
<img src="https://render.githubusercontent.com/render/math?math=%5CvarDelta_%7B(4%2B5)(1%2B0)%7D%5E%7B(2%2B0)(1%2B0)%7D%20%3D-%20X20"><!-- $\varDelta_{(4+5)(1+0)}^{(2+0)(1+0)} =- X20$\ -->

The original results can be seen in [output file](./Models.txt)
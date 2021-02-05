# Primitive MIMO example

<!-- ![Circuit](3-RC-M-2.svg) -->

The circuit have 2 inputs \f$V_I\f$ and \f$SI_I\f$ and 2 outputs \f$V_O\f$ and \f$I_O\f$.

The common denominator is (voltage input, current output)

(1) \f$D=\Delta^{(1+0)(4+5)}_{(1+0)(4+5)}\f$


The numerators: Copy denominator, remove pair of input gates and output gate (if they exist in copy). Finally add a pair in a form: \f$\phantom{ }^{(a_I+b_I)}_{(a_O+b_O)}\f$.


(2) \f$N_{vo\leftarrow vi}=\Delta^{(1+0)(4+5)}_{(3+5)(4+5)}\f$

(3) \f$N_{vo\leftarrow ii}=\Delta^{(1+0)(4+5)(2+0)}_{(1+0)(4+5)(3+5)}\f$

(4) \f$N_{io\leftarrow vi}=\Delta^{(1+0)}_{(4+5)}\f$

(5) \f$N_{io\leftarrow ii}=\Delta^{(1+0)(2+0)}_{(1+0)(4+5)}\f$

(6) \f$K_{vo\leftarrow vi}={{\Delta^{(1+0)(4+5)}_{(3+5)(4+5)}}\over{\Delta^{(1+0)(4+5)}_{(1+0)(4+5)}}}=\left(A\cdot R_i\right){{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}\f$

(7) \f$M_{vo\leftarrow ii}={{\Delta^{(1+0)(4+5)(2+0)}_{(1+0)(4+5)(3+5)}}\over{\Delta^{(1+0)(4+5)}_{(1+0)(4+5)}}}=\left(A\cdot R_s\cdot R_i \right){{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}\f$

(8) \f$N_{io\leftarrow vi}={{\Delta^{(1+0)}_{(4+5)}}\over{\Delta^{(1+0)(4+5)}_{(1+0)(4+5)}}}={{A\cdot R_i}\over{R_o}}{{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}\f$

(9) \f$K_{io\leftarrow ii}={{\Delta^{(1+0)(2+0)}_{(1+0)(4+5)}}\over{\Delta^{(1+0)(4+5)}_{(1+0)(4+5)}}}={{A\cdot R_s\cdot R_i}\over{R_o}}{{-1}\over{s\cdot\left(R_f+R_s \right)\cdot\left(\left(A+1\right)\cdot C_f+C_i \right)\cdot R_i+\left(R_f+R_s+R_i \right)}}\f$

However, the calculation can be more effective if we represent the above formula as SoE:

\f$X10=R_f+R_s+R_i\f$

\f$X11=X10\cdot R_o\f$

\f$X12=A+1\f$

\f$X13=X12\cdot C_f\f$

\f$X14=X13+C_i\f$

\f$X15=R_f+R_s\f$

\f$X16=X15\cdot X14\cdot R_i\cdot R_o\f$

\f$X17=A\cdot R_i\cdot R_o\f$

\f$X18=A\cdot R_s\cdot R_i\cdot R_o\f$

\f$X19=A\cdot R_i\f$

\f$X20=A\cdot R_s\cdot R_i\f$

\f$\Delta_{(1+0)(4+5)}^{(1+0)(4+5)}=s\cdot X16+X11\f$

\f$\Delta_{(3+5)(4+5)}^{(1+0)(4+5)} =- X17\f$

\f$\Delta_{(3+5)(1+0)(4+5)}^{(2+0)(1+0)(4+5)} =- X18\f$

\f$\Delta_{(4+5)}^{(1+0)}=- X19\f$

\f$\Delta_{(4+5)(1+0)}^{(2+0)(1+0)} =- X20\f$

The original results can be seen in [output file](./Models.txt)\f
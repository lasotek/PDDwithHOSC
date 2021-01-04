# Natural behavioral model made of controled sources – the asymetric external circuit

In literature there can be found two version of FDCFOA impementation. All of them have CFMB at the input. It is necesarry to immediate removig of a larg common signal at the output. However, the output can be different. In some application instead of fully differential buffer, we can find 2 separate buffers: one for positive and another for negative voltages around the device reference node (e.g. **M. Kumngern, F. Khateb, and T. Kulej, “0.5 V bulk-driven CMOS fully differential current feedback operational amplifier,”** *IET Circuits,Devices Syst.* **, vol. 13, no. 3, pp. 314–320, may 2019**). Thus, there are two different models tested:

![Two_CFMD](../ComSVG/FDCFOA_2CFMB_Model.svg)

*Model with CFMB at the output (Model 1)*

![One_CDMB](../ComSVG/FDCFOA_1CFMB_Model.svg)

*Model without CFMB at the output (Model 2)*

We tested them in circuits:

![K_u1](../ComSVG/FDCFOA_Gen_GGnd_Asym_K_u1.svg)


![K_u2](../ComSVG/FDCFOA_Gen_GGnd_Asym_K_u2.svg)


![K_u_com](../ComSVG/FDCFOA_Gen_GGnd_Asym_K_uc.svg)



There four set of results:
1. Idealized (no tracking errors) with Model 1: in [row text form](./Models_CFMB_OUT.txt) and [post-processed pdf](./Models_CFMB_OUT.pdf).
2. With tracking error consideration and Model 1: in [row text form](./ModelsTE_CFMB.txt) and [post-processed pdf](./ModelsTE_CFMB.pdf).
3. Idealized (no tracking errors) with Model 2: in [row text form](./Models_NO_CFMB_OUT.txt) and [post-processed pdf](./Models_NO_CFMB_OUT.pdf).
4. With tracking error consideration and Model 2: in [row text form](./ModelsTE_NO_CFMB.txt) and [post-processed pdf](./ModelsTE_NO_CFMB.pdf).

However, in contrast to examples in the [previous test](../13.%20SimpModelControlledSources/), we assume that there are no the same value external components, and how it spoils our circuits. The components with the same nominal values usually are slightly different, unless we perform expensive operation of pairing components.

Just like in the [previous point](../13.%20SimpModelControlledSources/), results show that tracking errors can be compensated by proper choice of resistors *R* and *kR*. The results can be simpler if we assume that *R<sub>m</sub>* tends to infinity, which is reasonable.

The asymmetry additionally shows how to obtain the common gain equal to 0. We must ensure that the time constants *R1a⋅C1a* and *R1b⋅C1b* are always equal.

Models are simplification of the actual structure that shows each possible circuit behavior, even if it is connected in uncommon way (the model from [10](../10.%20SimpModelSanchez-Lopez/) and [11](../11.%20SimpModelSanchez-LopezImproved/) is not so flexible). Connecting *R* and *kR* reduces model significantly. 

The same results, but for asymetric external circuit can be seen in [14](../14.%20SimpModelControlledSourcesAsym/).
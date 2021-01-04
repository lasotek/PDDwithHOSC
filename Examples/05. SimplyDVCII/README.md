# DVCII based voltage filter

Differential voltage second generation current conveyors can be modeled as 

![DVCII](DVCIIModel.svg)

in the natural way from its behavioral description. To consider tracking errors we should introduce voltage gains (α<sub>1</sub> and α<sub>2</sub>) and current gain (β) instead of followers. In case of the idealized model we have *null-descendant meta-vertex* ![M\left(+\phantom{ }_{(y_p+y_n)}^{(z_n+z_p)}-\phantom{ }_{(x+0)}^{(z_n+z_p)}+\phantom{ }_{(x+0)}^{(x+0)}-\phantom{ }_{(y_p+y_n)}^{(x+0)}\right)](https://render.githubusercontent.com/render/math?math=M%5Cleft(%2B%5Cphantom%7B%20%7D_%7B(y_p%2By_n)%7D%5E%7B(z_n%2Bz_p)%7D-%5Cphantom%7B%20%7D_%7B(x%2B0)%7D%5E%7B(z_n%2Bz_p)%7D%2B%5Cphantom%7B%20%7D_%7B(x%2B0)%7D%5E%7B(x%2B0)%7D-%5Cphantom%7B%20%7D_%7B(y_p%2By_n)%7D%5E%7B(x%2B0)%7D%5Cright))

For filter

![Filter](DVCIIFilter.svg)

we obtain the fallowing simply PDD

![PDD](DVCIIPDD.svg)

The detailed result can be seen in [raw text](Models%20Ideal.txt) or [post-processed pdf](Models%20Ideal.pdf). 

For models with tracking errors we can the result in [raw text](Models%20TE.txt) or or [post-processed pdf](Models%20TE.pdf) in case α<sub>1</sub>=α<sub>2</sub>, and in [raw text](Models%20TEA.txt) or or [post-processed pdf](Models%20TEA.pdf), otherwise.
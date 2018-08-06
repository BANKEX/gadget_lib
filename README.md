# BANKEX Foundation gadget-lib for zkSNARKs.

# RESEARCH PROJECT, WIP, NOT READY FOR PRODUCTION

# What are SNARKs and the gadget-lib

There are various explainers about what are zkSNARKs, such as Vitalik's personal blog or a wonderful web-site [qed-it](https://qed-it.com). Here we will try to explain what are the practical aspects of zkSNARKs and what tools are necessary for better adoption of it.

## zkSNARKs

In many cases user has some input data `x` and output `y` with some relation `y = f(x)`. Imagine that `f(x)` is a complicated long function that can be barely executed in the smart-contract, although everyone is only interested in a correct `y` for some `x`. zkSNARKs are here for rescue with a paradigm shift:
- Everyone (remember, we are on a public chain) agrees on the nature of `f(x)` after some setup procedure
- Someone can calculate `y = f(x)` locally
- This entity can publish just `y` and `x` values to the smart-contract without explicit `f` function execution
- The same entity must instead prepare and publish a PROOF that indeed for the published `x` and `y` holds a relationship `y == f(x)` where `==` means not equality
- Such a proof has nice properties: it has constant size and verification time is also constant

So instead of the explicit computation of `f` in a smart-contract we've saved a lot of gas (for large complicated functions) by computing the result locally and proving that we've calculated it correctly. Here we use SNARKs for proof of correctness.

Another good example of zkSNARKs in terms of zero knowledge aspect is more involved. One can prove that he KNOWS `x` value such that `y == f(x)`. Examples of such functions are ones where a user would want to prove KNOWLEDGE of `x` for some `y` - knowledge of hash preimage, knowledge of some element in the Merkle tree and other forms of commitments. zCash uses such functionality for their confidential transactions.

Of course there are intermediate cases where one wants to particlly disclose `x`, for example.

## Gadget-libs

Internally zkSNARK operation procedure requires conversion of `f(x)` into the special form of "rank one constraint system" (R1CS). To lift this conversion difficulty from the shoulders of the users various "gadget libraries" exist, for example `libsnark` provides two. An example of such gadget can be conversion of the standard hash function SHA256 into some gadget with 1 input (hash preimage) and 1 output (produced hash). Internally such gadget has more than 20000 constraints.

Some gadgets may place certrain requirements on their inputs or guarantee some properties of the outputs. For example, a XOR gadget may require inputs to be separate "bits" (either 0 or 1) and may produce an output in a form of bits too.

Our vision is that gadget-lib should take care of such requirements and verify them internally. At the moment we check this requirements in the runtime - our library will allow you to connect gadgets in your code without satisfaction of such requirements, although will crash with indication of error to allow user to modify his assembly of gadget and make a proper SNARK. 

We have chosen a "runtime" option for a sake of integration - after some point all the high-level functionality will be wrapped in Python and may be some other languages to reduce the very high entrance level for work with the SNARKs and introduce everyone with a nice toolset. One can think about it as a "Tensorflow" for zkSNARKs.

## Authors

Konstantin Panarin, [@Konstantce](https://github.com/Konstantce)

Alex Vlasov, [@shamatar](https://github.com/shamatar), alex.m.vlasov@gmail.com

## License

All the original code in this repository is available under the Apache License 2.0 license. See the [LICENSE](https://github.com/BankEx/gadget_lib/blob/master/LICENSE) file for more info.

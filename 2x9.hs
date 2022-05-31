import Data.Vector

fun x = generate 10 (*x)

main = loop 2
          where loop 10 = return ()
                loop x  = print (fun x) >> loop (x+1)

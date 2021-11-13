import Control.Concurrent

main::IO ()
main = for 0 (threadDelay 100000) >> (threadDelay 100000)
    where for c fun = if c>10 then return () else forkIO (putStrLn $ "thread!"++(show c)) >> (for (c+1) fun) 

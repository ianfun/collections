{-# LANGUAGE ForeignFunctionInterface #-}
import Foreign.C.Types
import Foreign.C.String

foreign import ccall unsafe "puts" puts::CString->IO CInt
foreign import ccall unsafe "getchar" getchar::IO CInt
main = do
    a <- newCString "Hello world"
    b <- puts a
    c <- getchar
    putStrLn $ "I get the char " ++ (show c)
{-
$ ./main
Hello world
A
I get the char 65   
-}

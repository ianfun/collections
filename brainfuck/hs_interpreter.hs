import System.Exit
import System.IO (hFlush, stdout)
import Data.Word (Word8)
import Data.Char (chr, ord)
data TYPE = TYPE{getbyte::[Word8], getptr::Int} deriving Show
maintype::TYPE
maintype = TYPE {getbyte=[(0::Word8) | _ <- [(0::Int)..(199::Int)]], getptr=0}

(*>*)::TYPE->TYPE
(*>*) x = TYPE {getbyte=(getbyte x), getptr=(getptr x)+1}
(*<*)::TYPE->TYPE
(*<*) x = TYPE {getbyte=(getbyte x), getptr=(getptr x)-1}
modify::TYPE->(Word8->Word8)->TYPE
modify x fun = let index=getptr x; arr=getbyte x
            in TYPE {getptr=index, getbyte=(take index arr)++((fun (arr!!index)):[])++(drop (index+1) arr)}

(*+*)::TYPE->TYPE
(*+*) x = modify x (1+)
(*-*)::TYPE->TYPE
(*-*) x = modify x (1-)
(*.*)::TYPE->IO ()
(*.*) x = (putChar $ (chr . fromEnum) ((getbyte x)!!(getptr x)) ) >> hFlush stdout
(*?*)::TYPE->IO TYPE
(*?*) x = getChar >>= \a->return (modify x  (\_->(toEnum (ord a))::Word8))

main::IO ()
main = 
    iter "+[,.]\0" 0 maintype 0
    where 
            iter code pc btype jumpback = case code!!pc of
                                '\x0'-> exitWith ExitSuccess
                                '>' -> iter code (pc+1) ((*>*) btype) jumpback
                                '<' -> iter code (pc+1) ((*<*) btype) jumpback
                                '+' -> iter code (pc+1) ((*+*) btype) jumpback
                                '-' -> iter code (pc+1) ((*-*) btype) jumpback
                                '.' -> (*.*) btype >> iter code (pc+1) btype jumpback
                                ',' -> (do 
                                            a <- (*?*) btype
                                            iter code (pc+1) a jumpback)
                                '[' -> if ((getbyte btype)!!(getptr btype)==0) then iter code ((until (\x->(code!!x)==']') (+1) pc)+1) btype jumpback else iter code (pc+1) btype pc
                                ']' -> if ((getbyte btype)!!(getptr btype)==0) then iter code (pc+1) btype jumpback else iter code (pc - jumpback - 1) btype jumpback
                                c -> putStr "warning: invalid char " >> putChar c >> putChar '\n' >> iter code (pc+1) btype jumpback

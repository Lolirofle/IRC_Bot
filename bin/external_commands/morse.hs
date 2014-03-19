import qualified System.IO.UTF8 as UTF8.IO
import Data.Char
import Data.List

sign_gap = ""
char_gap = " "
word_gap = " "

data MorseSignal = Short | Long

morse_char :: [MorseSignal] -> String
morse_char sequence = concatMap toStr sequence
	where toStr Short = "·"
	      toStr Long  = "-"

morse :: String -> String
morse str = intercalate char_gap (map toMorse str)
	where
		toMorse :: Char -> String
		toMorse char = case toLower char of
			' ' -> word_gap
			'a' -> morse_char [Short,Long]
			'b' -> morse_char [Long,Short,Short,Short]
			'c' -> morse_char [Long,Short,Long,Short]
			'd' -> morse_char [Long,Short,Short]
			'e' -> morse_char [Short]
			'f' -> morse_char [Short,Short,Long,Short]
			'g' -> morse_char [Long,Long,Short]
			'h' -> morse_char [Short,Short,Short,Short]
			'i' -> morse_char [Short,Short]
			'j' -> morse_char [Short,Long,Long,Long]
			'k' -> morse_char [Long,Short,Long]
			'l' -> morse_char [Short,Long,Short,Short]
			'm' -> morse_char [Long,Long]
			'n' -> morse_char [Long,Short]
			'o' -> morse_char [Long,Long,Long]
			'p' -> morse_char [Short,Long,Long,Short]
			'q' -> morse_char [Long,Long,Short,Long]
			'r' -> morse_char [Short,Long,Short]
			's' -> morse_char [Short,Short,Short]
			't' -> morse_char [Long]
			'u' -> morse_char [Short,Short,Long]
			'v' -> morse_char [Short,Short,Short,Long]
			'w' -> morse_char [Short,Long,Long]
			'x' -> morse_char [Long,Short,Short,Long]
			'y' -> morse_char [Long,Short,Long,Long]
			'z' -> morse_char [Long,Long,Short,Short]
			
			'å' -> morse_char [Short,Long,Long,Short,Long]
			'ä' -> morse_char [Short,Long,Short,Long]
			'ö' -> morse_char [Long,Long,Long,Short]

			'0' -> morse_char [Long,Long,Long,Long,Long]
			'1' -> morse_char [Short,Long,Long,Long,Long]
			'2' -> morse_char [Short,Short,Long,Long,Long]
			'3' -> morse_char [Short,Short,Long,Long,Long]
			'4' -> morse_char [Short,Short,Short,Long,Long]
			'5' -> morse_char [Short,Short,Short,Short,Long]
			'6' -> morse_char [Long,Short,Short,Short,Short]
			'7' -> morse_char [Long,Long,Short,Short,Short]
			'8' -> morse_char [Long,Long,Long,Short,Short]
			'9' -> morse_char [Long,Long,Long,Long,Short]
			
			'.' -> morse_char [Short,Long,Short,Long,Short,Long]
			',' -> morse_char [Long,Long,Short,Short,Long,Long]
			'?' -> morse_char [Short,Short,Long,Long,Short,Short]
			'\'' -> morse_char[Short,Long,Long,Long,Long,Short]
			'!' -> morse_char [Long,Short,Long,Short,Long,Long]
			'/' -> morse_char [Long,Short,Short,Long,Short]
			'(' -> morse_char [Long,Short,Long,Long,Short]
			')' -> morse_char [Long,Short,Long,Long,Short,Long]
			'&' -> morse_char [Short,Long,Short,Short,Short]
			':' -> morse_char [Long,Long,Long,Short,Short,Short]
			';' -> morse_char [Long,Short,Long,Short,Long,Short]
			'=' -> morse_char [Long,Short,Short,Short,Long]
			'+' -> morse_char [Short,Long,Short,Long,Short]
			'-' -> morse_char [Long,Short,Short,Short,Short,Long]
			'_' -> morse_char [Short,Short,Long,Long,Short,Long]
			'"' -> morse_char [Short,Long,Short,Short,Long,Short]
			'$' -> morse_char [Short,Short,Short,Long,Short,Short,Long]
			'@' -> morse_char [Short,Long,Long,Short,Long,Short]
			_   -> ['(',char,')']

main = interact morse

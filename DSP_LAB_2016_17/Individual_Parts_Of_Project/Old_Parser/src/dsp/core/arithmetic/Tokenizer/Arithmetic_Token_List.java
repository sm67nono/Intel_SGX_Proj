package dsp.core.arithmetic.Tokenizer;

import java.util.LinkedList;
import java.util.regex.Pattern;

import dsp.core.arithmetic.parser.Exp_Function;


public class Arithmetic_Token_List {
	private LinkedList<Token_Info> t_Info;
	
	public LinkedList<Token_Info> getT_Info() {
		return t_Info;
	}


	//Initialize Arithmetic_Parser's List
	public Arithmetic_Token_List() {
		t_Info= new LinkedList<Token_Info>();
		
	}
	
	
	//Creating the the Token_info list
		public void addtolist(String inp_regex, int token_ID) {
			t_Info.add(new Token_Info(Pattern.compile("^("+inp_regex+")"), token_ID));
		}
	
	//Creating Terminal IDs for the Grammar of arithmetic parser
	public LinkedList<Token_Info> setParserGrammerArith()
	{
		
		/*addtolist("expo|exponent|exp|ln|sqr|sqrt|square|cube|sin|cos|tan", 1);
		addtolist("\\(", 2);
		addtolist("\\)", 3); 
		addtolist("\\+|-", 4);
		addtolist("\\*|/", 5);
		addtolist("\\^", 6);
		addtolist("[0-9]+",7);
		addtolist("[a-zA-Z][a-zA-Z0-9_]*", 8); */
		String funcs = Exp_Function.get_all_defined_Functions();
		addtolist("[+-]", Tokens.PLUSMINUS);
		addtolist("[*/]", Tokens.MULTDIV);
		addtolist("\\^", Tokens.RAISED);  
		addtolist("(" + funcs + ")(?!\\w)", Tokens.FUNCTION);
		addtolist("\\(", Tokens.OPEN_BRACKET);
		addtolist("\\)", Tokens.CLOSE_BRACKET);
		addtolist("(?:\\d+\\.?|\\.\\d)\\d*(?:[Ee][-+]?\\d+)?", Tokens.NUMBER);
		addtolist("[a-zA-Z]\\w*", Tokens.VARIABLE);
		
		return t_Info;
	}
	

}

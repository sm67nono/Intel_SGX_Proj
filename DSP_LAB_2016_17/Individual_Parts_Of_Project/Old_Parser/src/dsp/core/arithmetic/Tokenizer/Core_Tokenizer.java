package dsp.core.arithmetic.Tokenizer;


import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Core_Tokenizer {

	private LinkedList<Token_Info> t_Info;
	public LinkedList<Token_Info> getT_Info() {
		return t_Info;
	}

	public void setT_Info(LinkedList<Token_Info> t_Info) {
		this.t_Info = t_Info;
	}

	public void setTokens(LinkedList<Tokens> tokens) {
		this.tokens = tokens;
	}



	private LinkedList<Tokens> tokens;

	//Initialize Core_Tokenizer
	public Core_Tokenizer() {
		t_Info = new LinkedList<Token_Info>();
		tokens = new LinkedList<Tokens>();
	}

	public LinkedList<Tokens> getTokens() {
		  return tokens;
		}


	//To tokenize an input string
	public void tokenize_the_input(String input_str) {
		String new_str = new String(input_str.trim());
		tokens.clear();// clearing the list if values stored previously

		while (!new_str.equals("")) {
			boolean check = false;
            
			for (Token_Info get_info : t_Info) {
				//Match doesn't work if there are spaces in input so input has to be sanitized
				Matcher match = get_info.getReg_Exp().matcher(new_str);
				if (match.find()) {
					check = true;
					String matched_token = match.group().trim();
					tokens.add(new Tokens(get_info.getToken_ID(), matched_token));
					//Matching fix by adding trim in the end
					new_str = match.replaceFirst("").trim();
					break;
				}
			}
		}
	}
	


}

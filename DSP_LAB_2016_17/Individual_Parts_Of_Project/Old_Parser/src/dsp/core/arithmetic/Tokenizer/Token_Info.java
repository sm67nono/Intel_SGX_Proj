package dsp.core.arithmetic.Tokenizer;
import java.util.regex.Pattern;

public class Token_Info {
	 	private Pattern reg_Exp;
	    private int token_ID;
	    
	    
	    public Pattern getReg_Exp() {
			return reg_Exp;
		}


		public void setReg_Exp(Pattern reg_Exp) {
			this.reg_Exp = reg_Exp;
		}


		public int getToken_ID() {
			return token_ID;
		}


		public void setToken_ID(int token_ID) {
			this.token_ID = token_ID;
		}


		public Token_Info(Pattern reg_Exp, int token_ID) {
	        this.reg_Exp = reg_Exp;
	        this.token_ID = token_ID;
	      }

}

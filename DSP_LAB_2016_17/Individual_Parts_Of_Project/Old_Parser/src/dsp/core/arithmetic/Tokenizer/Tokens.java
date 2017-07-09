package dsp.core.arithmetic.Tokenizer;

public class Tokens {
	
	 
	 
	  public static final int PLUSMINUS = 1;
	 
	  public static final int MULTDIV = 2;
	  
	  public static final int RAISED = 3;
	 
	  public static final int FUNCTION = 4;
	 
	  public static final int OPEN_BRACKET = 5;
	  
	  public static final int CLOSE_BRACKET = 6;
	  
	  public static final int NUMBER = 7;
	  
	  public static final int VARIABLE = 8;
	  
	  public static final int EPSILON = 0;
	
	  public int token_ID;
	  public String seq;
	  public int token_pos;
	  
	 public Tokens(int token_ID, String seq, int token_pos) {
		    
		    this.token_ID = token_ID;
		    this.seq = seq;
		
		    this.token_pos=token_pos;
		    
		  }
		    

	  public Tokens(int token_ID, String seq) {
	    
	    this.token_ID = token_ID;
	    this.seq = seq;
	  }
	    
	 

}

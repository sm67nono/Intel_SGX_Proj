package dsp.core.arithmetic.Tokenizer;


public class ParserException extends RuntimeException {
	
	private Tokens token = null;
  public ParserException(String msg) {
    super(msg);
  }
  
  public ParserException(String message, Tokens token)
  {
    super(message);
    this.token = token;
  }
  
 
}
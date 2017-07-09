package Test_App;

import dsp.core.arithmetic.Tokenizer.ParserException;
import dsp.core.arithmetic.parser.EvaluationException;
import dsp.core.arithmetic.parser.Exp_Node;
import dsp.core.arithmetic.parser.Parser_Arithmetic;

public class Test_Main {
	
	  public static void main(String args[])
	  {
	    
	    String inp_str = "sin(10)+ln(8)+5+8-9";

	    Parser_Arithmetic parser = new Parser_Arithmetic();
	    try
	    {
	      Exp_Node expr = parser.parse(inp_str);
	      System.out.println("The value of the expression is "+expr.get_ExpNodeValue());
	      
	    }
	    catch (ParserException e)
	    {
	      System.out.println(e.getMessage());
	    }
	    catch (EvaluationException e)
	    {
	      System.out.println(e.getMessage());
	    }
	  }
	
}

package dsp.core.arithmetic.parser;

public interface Exp_Node_StepOn
{
	  
	  public void stepOn(Exp_Variable expVar);

	  
	  public void stepOn(Exp_Constant expConst);

	  
	  public void stepOn(Exp_Addition expAddition);

	 
	  public void stepOn(Exp_Multiply expMultiply);

	 
	  public void stepOn(Exp_Exponent expExponent);

	 
	  public void stepOn(Exp_Function expFunction);
}



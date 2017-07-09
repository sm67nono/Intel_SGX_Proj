package dsp.core.arithmetic.parser;

public class Exp_Multiply extends Exp_Sequence
{
	
	  public Exp_Multiply()
	  {}

	 
	  public Exp_Multiply(Exp_Node inp, boolean value)
	  {
	    super(inp, value);
	  }


	 
	@Override
	public int get_ExpNodeType() {
		// TODO Auto-generated method stub
		 return Exp_Node.MULTIPY_EXP;
	}

	@Override
	public double get_ExpNodeValue() {
		// TODO Auto-generated method stub
		 double product = 1.0;
		    for (Term t : list_term)
		    {
		      if (t.value)
		    	  product *= t.expr.get_ExpNodeValue();
		      else
		    	  product /= t.expr.get_ExpNodeValue();
		    }
		    return product;
	}

	@Override
	public void accept_step(Exp_Node_StepOn step) {
		// TODO Auto-generated method stub
		step.stepOn(this);  
		    for (Term t: list_term)
		      t.expr.accept_step(step);
		
	}

}

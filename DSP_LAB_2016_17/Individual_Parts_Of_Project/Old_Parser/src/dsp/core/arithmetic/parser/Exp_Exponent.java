package dsp.core.arithmetic.parser;


public class Exp_Exponent implements Exp_Node
{
	 
	  private Exp_Node term_base;
	 
	  private Exp_Node term_exponent;

	 
	  public Exp_Exponent(Exp_Node term_base, Exp_Node term_exponent)
	  {
	    this.term_base = term_base;
	    this.term_exponent = term_exponent;
	  }

	  

	@Override
	public int get_ExpNodeType() {
		// TODO Auto-generated method stub
		 return Exp_Node.EXPONENT_EXP;
	}

	@Override
	public double get_ExpNodeValue() {
		// TODO Auto-generated method stub
		return Math.pow(term_base.get_ExpNodeValue(), term_exponent.get_ExpNodeValue());
	}

	@Override
	public void accept_step(Exp_Node_StepOn step) {
		// TODO Auto-generated method stub
		step.stepOn(this);
		term_base.accept_step(step);
		term_exponent.accept_step(step);
		
	}
}
package dsp.core.arithmetic.parser;

public class Exp_Constant implements Exp_Node
{
	 
	  private double val;

	  public Exp_Constant(double val)
	  {
	    this.val = val;
	  }

	  public Exp_Constant(String val)
	  {
	    this.val = Double.valueOf(val);
	  }

	  


	@Override
	public int get_ExpNodeType() {
		// TODO Auto-generated method stub
		  return Exp_Node.CONS_EXP;
	}

	@Override
	public double get_ExpNodeValue() {
		// TODO Auto-generated method stub
		return val;
	}

	@Override
	public void accept_step(Exp_Node_StepOn step) {
		// TODO Auto-generated method stub
		step.stepOn(this);
		
	}
	
	}


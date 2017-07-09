package dsp.core.arithmetic.parser;

public class Exp_Variable implements Exp_Node
{
	  
	  private String variable_name;
	  
	  private double variable_val;
	  
	  private boolean variable_isValueSet;

	
	  public Exp_Variable(String variable_name)
	  {
	    this.variable_name = variable_name;
	    variable_isValueSet = false;
	  }

	 
	  public String getVariable_name()
	  {
	    return variable_name;
	  }

	 
	  public void setVariable_Value(double variable_val)
	  {
	    this.variable_val = variable_val;
	    this.variable_isValueSet = true;
	  }

	

	  

	@Override
	public int get_ExpNodeType() {
		// TODO Auto-generated method stub
		return Exp_Node.VAR_EXP;
	}

	@Override
	public double get_ExpNodeValue() {
		// TODO Auto-generated method stub
		 if (variable_isValueSet)
		      return variable_val;
		    else
		      throw new EvaluationException("Not Initialized" + variable_name );
	}

	@Override
	public void accept_step(Exp_Node_StepOn step) {
		// TODO Auto-generated method stub
		step.stepOn(this);
		
	}

}

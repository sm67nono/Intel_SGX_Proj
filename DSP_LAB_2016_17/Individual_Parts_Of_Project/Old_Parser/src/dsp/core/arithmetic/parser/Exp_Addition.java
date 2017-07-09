package dsp.core.arithmetic.parser;

public class Exp_Addition extends Exp_Sequence
{

 
  public Exp_Addition()
  {}

  
  public Exp_Addition(Exp_Node exp_node, boolean value)
  {
    super(exp_node, value);
  }



@Override
public int get_ExpNodeType() {
	// TODO Auto-generated method stub
	 return Exp_Node.ADD_EXP;
}


@Override
public double get_ExpNodeValue() {
	// TODO Auto-generated method stub
	double sum = 0.0;
    for (Term t : list_term)
    {
      if (t.value)
        sum += t.expr.get_ExpNodeValue();
      else
        sum -= t.expr.get_ExpNodeValue();
    }
    return sum;
  }



@Override
public void accept_step(Exp_Node_StepOn step) {
	// TODO Auto-generated method stub
	step.stepOn(this);
	    for (Term t : list_term)
	      t.expr.accept_step(step);
	
}

}
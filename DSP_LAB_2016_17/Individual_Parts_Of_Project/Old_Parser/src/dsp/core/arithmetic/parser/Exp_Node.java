package dsp.core.arithmetic.parser;

public interface Exp_Node
{
  
  public static final int VAR_EXP = 1;
  
  public static final int CONS_EXP = 2;
  
  public static final int ADD_EXP = 3;

  public static final int MULTIPY_EXP = 4;

  public static final int EXPONENT_EXP = 5;

  public static final int FUNC_EXP = 6;

  public int get_ExpNodeType();
  
  
  public double get_ExpNodeValue();
  
  
  public void accept_step(Exp_Node_StepOn step);

}
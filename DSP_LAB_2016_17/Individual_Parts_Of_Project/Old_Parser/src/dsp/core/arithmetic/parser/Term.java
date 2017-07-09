package dsp.core.arithmetic.parser;

public class Term
{
 
  public boolean value;

  public Exp_Node expr;

 
  public Term(boolean value, Exp_Node expr)
  {
    
    this.value = value;
    this.expr = expr;
  }
}
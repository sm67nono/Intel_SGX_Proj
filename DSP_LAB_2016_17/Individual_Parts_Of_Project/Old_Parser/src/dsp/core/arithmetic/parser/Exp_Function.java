package dsp.core.arithmetic.parser;

import dsp.core.arithmetic.Tokenizer.ParserException;

public class Exp_Function implements Exp_Node
{
	 
	  public static final int SIN = 1;
	  
	  public static final int COS = 2;
	  
	  public static final int TAN = 3;

	 
	  public static final int ASIN = 4;
	 
	  public static final int ACOS = 5;
	 
	  public static final int ATAN = 6;

	
	  public static final int SQRT = 7;
	
	  public static final int EXP = 8;

	
	  public static final int LN = 9;
	 
	  public static final int LOG = 10;
	  
	  public static final int LOG2 = 11;
	 
	  public static final int ABS = 12;

	 
	  private int func;

	
	  private Exp_Node args;
	  
      //Define the functions here in the string and in str2Func
	  public static String get_all_defined_Functions()
	  {
	    return "sin|cos|tan|asin|acos|atan|sqrt|exp|ln|log|log2|abs";
	  }

	  public Exp_Function(int func, Exp_Node args)
	  {
	  
	    this.func = func;
	    this.args = args;
	  }

	 
	  
	  public static int str2Func(String inp_str)
	  {
	    if (inp_str.equals("sin"))
	      return Exp_Function.SIN;
	    if (inp_str.equals("cos"))
	      return Exp_Function.COS;
	    if (inp_str.equals("tan"))
	      return Exp_Function.TAN;

	    if (inp_str.equals("asin"))
	      return Exp_Function.ASIN;
	    if (inp_str.equals("acos"))
	      return Exp_Function.ACOS;
	    if (inp_str.equals("atan"))
	      return Exp_Function.ATAN;

	    if (inp_str.equals("sqrt"))
	      return Exp_Function.SQRT;
	    if (inp_str.equals("exp"))
	      return Exp_Function.EXP;

	    if (inp_str.equals("ln"))
	      return Exp_Function.LN;
	    if (inp_str.equals("log"))
	      return Exp_Function.LOG;
	    if (inp_str.equals("log2"))
	      return Exp_Function.LOG2;
	    if (inp_str.equals("abs"))
	        return Exp_Function.ABS;

	    throw new ParserException("Function " + inp_str + " not found. Cannot be Parsed.");
	  }
	  
	@Override
	public int get_ExpNodeType() {
		// TODO Auto-generated method stub
		return Exp_Function.FUNC_EXP;
	}

	@Override
	public double get_ExpNodeValue() {
		// TODO Auto-generated method stub
		 switch (func)
		    {
		      case SIN:
		        return Math.sin(args.get_ExpNodeValue());
		      case COS:
		        return Math.cos(args.get_ExpNodeValue());
		      case TAN:
		        return Math.tan(args.get_ExpNodeValue());
		      case ASIN:
		        return Math.asin(args.get_ExpNodeValue());
		      case ACOS:
		        return Math.acos(args.get_ExpNodeValue());
		      case ATAN:
		        return Math.atan(args.get_ExpNodeValue());
		      case SQRT:
		        return Math.sqrt(args.get_ExpNodeValue());
		      case EXP:
		        return Math.exp(args.get_ExpNodeValue());
		      case LN:
		        return Math.log(args.get_ExpNodeValue());
		      case LOG:
		        return Math.log(args.get_ExpNodeValue()) * 0.43;
		      case LOG2:
		        return Math.log(args.get_ExpNodeValue()) * 1.44;
		      case ABS:
		          return Math.abs(args.get_ExpNodeValue());
		        
		    }

		    throw new EvaluationException("Function ID incorrect : "+func);
	}

	@Override
	public void accept_step(Exp_Node_StepOn step) {
		// TODO Auto-generated method stub
		step.stepOn(this);
	    args.accept_step(step);
	}
}

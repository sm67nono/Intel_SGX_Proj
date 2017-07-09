package dsp.core.arithmetic.parser;

import java.util.LinkedList;

import dsp.core.arithmetic.Tokenizer.Arithmetic_Token_List;
import dsp.core.arithmetic.Tokenizer.Core_Tokenizer;
import dsp.core.arithmetic.Tokenizer.ParserException;
import dsp.core.arithmetic.Tokenizer.Tokens;



public class Parser_Arithmetic
{
  
	LinkedList<Tokens> tkns;
	Tokens look_ahead;
	 
	  
	  public Exp_Node parse(String input_expr)
	  {
		Core_Tokenizer c_tokenizer = new Core_Tokenizer();
	    Arithmetic_Token_List atl = new Arithmetic_Token_List();
	    c_tokenizer.setT_Info(atl.setParserGrammerArith());
	    c_tokenizer.tokenize_the_input(input_expr);
	    LinkedList<Tokens> tok = c_tokenizer.getTokens();
	    return this.parse(tok);
	  }

	 
	 
	public Exp_Node parse(LinkedList<Tokens> tok)
	  {
	    
	    this.tkns = (LinkedList<Tokens>)tok.clone();
	    look_ahead = this.tkns.getFirst();

	   
	    Exp_Node expr = exprn();
	    
	    //The last token in the list must be EPSILON if not then some unrecognized symbol is present
	    if (look_ahead.token_ID != Tokens.EPSILON)
	      throw new ParserException("Symbol %s not recognized by parser", look_ahead);
	    
	    return expr;
	  }

	  
	  //LL1 Rules Defined
	
	  private Exp_Node exprn()
	  {
		 
	   
	    Exp_Node expr = signed_Term();
	    expr = sum_Op(expr);
	    return expr;
	  }

	
	  private Exp_Node sum_Op(Exp_Node expr)
	  {
	   
	    if (look_ahead.token_ID == Tokens.PLUSMINUS)
	    {
	      Exp_Addition sum;
	      
	      if (expr.get_ExpNodeType() == Exp_Node.ADD_EXP)
	        sum = (Exp_Addition) expr;
	      else
	        sum = new Exp_Addition(expr, true);

	      //Recursive Reduction
	      boolean value = look_ahead.seq.equals("+");
	      next_Token();
	      Exp_Node t = inp_term();
	      sum.add(t, value);

	      return sum_Op(sum);
	    }

	    
	    return expr;
	  }

	 
	  private Exp_Node signed_Term()
	  {
	   
	    if (look_ahead.token_ID == Tokens.PLUSMINUS)
	    {
	      boolean positive = look_ahead.seq.equals("+");
	      next_Token();
	      Exp_Node t = inp_term();
	      if (positive)
	        return t;
	      else
	        return new Exp_Addition(t, false);
	    }

	    
	    return inp_term();
	  }

	 
	  private Exp_Node inp_term()
	  {
	   
	    Exp_Node fact = term_factor();
	    return term_Ops(fact);
	  }

	
	  private Exp_Node term_Ops(Exp_Node expr)
	  {
	    
	    if (look_ahead.token_ID == Tokens.MULTDIV)
	    {
	      Exp_Multiply product;

	    
	      if (expr.get_ExpNodeType() == Exp_Node.MULTIPY_EXP)
	    	  product = (Exp_Multiply) expr;
	      else
	    	  product = new Exp_Multiply(expr, true);

	     
	      boolean positive = look_ahead.seq.equals("*");
	      next_Token();
	      Exp_Node f = signed_Factor();
	      product.add(f, positive);

	      return term_Ops(product);
	    }

	   
	    return expr;
	  }
	  
	 
	  private Exp_Node signed_Factor()
	  {
	    
	    if (look_ahead.token_ID == Tokens.PLUSMINUS)
	    {
	      boolean value = look_ahead.seq.equals("+");
	      next_Token();
	      Exp_Node tfact = term_factor();
	      if (value)
	        return tfact;
	      else
	        return new Exp_Addition(tfact, false);
	    }

	   
	    return term_factor();
	  }
	  
	
	  private Exp_Node term_factor()
	  {
	   
	    Exp_Node args = args();
	    return factor_Op(args);
	  }
	  
	 
	  private Exp_Node factor_Op(Exp_Node expr)
	  {
	   
	    if (look_ahead.token_ID == Tokens.RAISED)
	    {
	      next_Token();
	      Exp_Node expo = signed_Factor();

	      return new Exp_Exponent(expr, expo);
	    }

	   
	    return expr;
	  }

	  
	  private Exp_Node args()
	  {
	    
	    if (look_ahead.token_ID == Tokens.FUNCTION)
	    {
	      int func = Exp_Function.str2Func(look_ahead.seq);
	      next_Token();
	      Exp_Node expression = args();
	      return new Exp_Function(func, expression);
	    }
	    
	    else if (look_ahead.token_ID == Tokens.OPEN_BRACKET)
	    {
	      next_Token();
	      Exp_Node expression = exprn();
	      if (look_ahead.token_ID != Tokens.CLOSE_BRACKET)
	        throw new ParserException("Missing a closing Brakcet", look_ahead);
	      next_Token();
	      return expression;
	    }

	    
	    return get_val();
	  }

	 
	  private Exp_Node get_val()
	  {
	   
	    if (look_ahead.token_ID == Tokens.NUMBER)
	    {
	      Exp_Node expression = new Exp_Constant(look_ahead.seq);
	      next_Token();
	      return expression;
	    }

	  
	    if (look_ahead.token_ID == Tokens.VARIABLE)
	    {
	      Exp_Node expression = new Exp_Variable(look_ahead.seq);
	      next_Token();
	      return expression;
	    }

	    if (look_ahead.token_ID == Tokens.EPSILON)
	      throw new ParserException("Input did not end properly");
	    else
	      throw new ParserException("%s Incorrect symbol found", look_ahead);
	  }

	  
	  private void next_Token()
	  {
	    tkns.pop();
	    
	    if (tkns.isEmpty())
	      look_ahead = new Tokens(Tokens.EPSILON, "", -1);
	    else
	      look_ahead = tkns.getFirst();
	  }
}
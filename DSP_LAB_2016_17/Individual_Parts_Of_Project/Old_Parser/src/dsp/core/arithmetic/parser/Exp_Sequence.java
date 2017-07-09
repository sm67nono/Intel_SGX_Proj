package dsp.core.arithmetic.parser;

import java.util.ArrayList;

public abstract class Exp_Sequence implements Exp_Node
	{
	  
	
	  protected ArrayList<Term> list_term;

	 
	  public Exp_Sequence()
	  {
	    this.list_term = new ArrayList<Term>();
	  }

	 
	  public Exp_Sequence(Exp_Node a, boolean value)
	  {
	    this.list_term = new ArrayList<Term>();
	    this.list_term.add(new Term(value, a));
	  }

	 
	  public void add(Exp_Node node, boolean value)
	  {
	    this.list_term.add(new Term(value, node));
	  }

	}
import java.io.*;
/**
*
*@author manna: DSP_LAB 2016_17
*
*/

class JavaApp
{
	static{
		System.loadLibrary("Enc_Eval");
	}

private native void showResult(String str);

public static void main(String[] args){
		//new JavaApp().showResult("(20+5)*10-(4*2)");
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		String s = "";
        System.out.println("Enter an Arithmetic Expression containing only [0-9], [*,+,-] and parentheses :");
		try
		{
			s = br.readLine();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		new JavaApp().showResult(s);
	}
}

import java.io.*;
import java.security.SecureRandom;
import java.util.Random;
import java.util.*;
import java.lang.*;
import java.util.Scanner;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Random;

public class Fuzzer {

     private Random random = new SecureRandom();
     private int count = 1;
     private final int SEGV = 139;
     private String targetExecutable="vuls1.cpp";
     public File muteFile;
     private int imglength;
     private int excode;

     public File mutate(File in, int start, int length) throws IOException  
     {
         //System.out.println("inp.length= "+ ((int) in.length())+" name= "+in.getName());
         //imglength = (int) in.length();
         byte[] data = new byte[(int) in.length()];
         DataInputStream din = new DataInputStream(new FileInputStream(in));
         din.readFully(data);
         //System.out.println("Byte stream: "+ din);
         fuzz(data, start, length);

         String name = "fuzz_" + count + "_" + in.getName();
         File fout = new File(name);
         FileOutputStream  out = new FileOutputStream(fout);
         out.write(data);
         out.close();
         din.close();
         count++;
         return fout;
     }
     // Modifies byte array in place
     public void fuzz(byte[] in, int start, int length) {
 
         byte[] fuzz = new byte[length];
         random.nextBytes(fuzz);
         System.arraycopy(fuzz, 0, in, start, fuzz.length);
     }
     //run target program with mutated image
     public void executeTarget(File testFile){
        char[] buffer = new char[1024];
       //run jpg2pdf.cpp with cola.jpg
       Runtime rt = Runtime.getRuntime();
       String[] converterAndmutimage = {"test3", testFile.getName()};
       Process p;
       String s = "";
       
        try {
            p = rt.exec(converterAndmutimage);
            //System.out.println("Done start!");
            BufferedReader err = new BufferedReader(new InputStreamReader(p.getErrorStream()));
			BufferedReader in = new BufferedReader(new InputStreamReader(p.getInputStream()));
            while (in.read(buffer) != -1)
              ; //System.out.println("buffer: "+ buffer);
			s = err.readLine();
            
			in.close();
			err.close();
            p.waitFor();

            //System.out.println("The exit code: "+ p.exitValue()+" s: "+s);
            excode = p.exitValue();
            
            //seg fault = 139. exit code==139 break from while loop
            if((excode != (-1)) && (excode != (0)) ){
                System.out.println("~~s index: "+excode);//+s.indexOf("BUG"));
                testFile.delete();
            }
            else{
                System.out.println("The exitcode: "+excode+" deleting: "+testFile.getName());
                testFile.delete();
            }
        }catch(Exception ex) {
            ex.printStackTrace();
        }
     }
     
     public static void main(String[] args) {
       Fuzzer hw = new Fuzzer();
       
       System.out.println("Enter img file name: ");
       Scanner scan = new Scanner(System.in);
       String filename = scan.next();
       //String filename = "contestA.in";
       //String filename = args[0];
       Random rand = new Random();
       File inp= new File(filename);
       hw.imglength = (int) inp.length();
       System.out.println("The file name: "+ inp.getName()+" "+hw.imglength);
       
       int ret,i=0;
       try{
         while (i<3){
            i++;
            int start = rand.nextInt(50) + 1; //max=10, min=1
            int len = rand.nextInt(1050) + 1;
            int range = start + len;
            if(range> hw.imglength) continue;

            hw.muteFile = hw.mutate(inp, start, len);
            System.out.println(i+ " Shown: "+ hw.muteFile + "start & len:"+start+" & "+len);
            hw.executeTarget(hw.muteFile);
            if(hw.excode==139) break;
          }
       }catch(IOException e){
         e.printStackTrace();
         System.out.println("Nothing..:] not mutated");
       }

    //    try{
    //      int i=0;
    //      while(i<3){
    //          i++;
    //          muteFile = hw.mutate(inp, 0, 10);
    //          hw.executeTarget(muteFile);
    //      }
         
    //     }catch(IOException e){
    //        e.printStackTrace();
    //     }
     

    }
 
}
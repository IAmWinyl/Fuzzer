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

    // Class variables
     private Random random = new SecureRandom();
     private Random rand = new Random();
     private int count = 1;
     private final int SEGV = 139;
     //private String targetExecutable="vuls1.cpp";
     public File muteFile;
     private int imglength;
     private int excode;
     int b = 0;

     // Description...
     public File mutate(File in, int start, int length) throws IOException  
     {
         //System.out.println("inp.length= "+ ((int) in.length())+" name= "+in.getName());

        // Allocate byte array for image
        byte[] data = new byte[(int) in.length()];
        // Fill array with image data
        DataInputStream din = new DataInputStream(new FileInputStream(in));
        din.readFully(data);

        // Alternate between....
        // Randomizing image file
        if (b==0){
            fuzz(data, start, length);
            b=1;
        }
        // Growing image file   
        else {
            // length 
            int len = rand.nextInt(99990) + 99980;
            data=fuzz1(data, len);
            b=0;
        }

        // Output image to file
        String name = "fuzz_" + count + "_" + in.getName();
        File fout = new File(name);
        FileOutputStream  out = new FileOutputStream(fout);
        out.write(data);

        out.close();
        din.close();

        // Increase file name count
        count++;

        return fout;
     }

     // Modifies byte array in place
     public void fuzz(byte[] in, int start, int length) {
        // Allocate image array
        byte[] fuzz = new byte[length];

        // Randomly fill with bytes
        random.nextBytes(fuzz);

        // Copy randomized array to main image array
        System.arraycopy(fuzz, 0, in, start, fuzz.length);
     }

     //Modifies by adding rand number of bytes
     public byte[] fuzz1(byte[] in, int length){
        // Allocate space to grow
        byte[] fuzz = new byte[length];
        byte[] dest = new byte[in.length + fuzz.length + fuzz.length + fuzz.length];

        // Fill with random bytes
        random.nextBytes(fuzz);

        int l = in.length - 1 + fuzz.length;

        // Copy original array to destination
        System.arraycopy(in, 0, dest, 0, in.length);
        // Concatenate destination and fuzz
        System.arraycopy(fuzz, 0, dest, in.length-1, fuzz.length);

        return dest;
     }

     public File shrinkFile(File in) throws IOException {
        
        // Allocate byte array for image
        byte[] data = new byte[(int) in.length()];

        // Fill array with image data
        DataInputStream din = new DataInputStream(new FileInputStream(in));
        din.readFully(data);

        // Assign position to end of array
        int pos = data.length - 1;
        byte[] newFile;

        // Find FFC0 or FFC2
        while(pos != 1) {
            // Find C0 or C2
            if(data[pos] == -64 || data[pos] == -62){    
                // Find FF
                if(data[pos-1] == -1) {  
                    break;
                }
            }
            // If not found, move position down the array
            pos--;
        }

        // Allocate new file
        newFile = new byte[pos+1];

        // Fill new file with old file's content up to FFC0 or FFC2
        System.arraycopy(data, 0, newFile, 0, pos+1);

        // Output image to file
        String name = "fuzz_" + count + "_" + in.getName();
        File fout = new File(name);
        FileOutputStream  out = new FileOutputStream(fout);
        out.write(newFile);

        out.close();
        din.close();

        // Increase file name count
        count++;

        return fout;
     }


     // Run target program with mutated image
     public void executeTarget(File testFile){
        char[] buffer = new char[1024];
       //run jpg2pdf.cpp with image.jpg
       Runtime rt = Runtime.getRuntime();
       String[] converterAndmutimage = {"test1", testFile.getName()};
       Process p;
       String s = "";
       
        try {
            p = rt.exec(converterAndmutimage);

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
            
            //seg fault = 139. exit code==139
            if((excode != (-1)) && (excode != (0)) && excode != (255)){
                System.out.println("~~Crashed ");
            }
            else{
                System.out.println("Did not crash- deleting: " + testFile.getName());
                //testFile.delete();
            }
        } catch(Exception ex) {
            ex.printStackTrace();
        }
     }
     
     public static void main(String[] args) {
        // Local variables
       int ret, i = 0;

       Fuzzer hw = new Fuzzer();
       
       // Read user input for image file name
       System.out.println("Enter img file name: ");
       Scanner scan = new Scanner(System.in);
       String filename = scan.next();
       
       // Open image
       File inp = new File(filename);

       // Get image length
       hw.imglength = (int) inp.length();
       System.out.println("The file name: " + inp.getName() + " " + hw.imglength);
        
        try {
            
            hw.executeTarget(hw.shrinkFile(inp));

            while (i<3) {
                i++;

                int start = hw.rand.nextInt(50) + 1; //max=10, min=1
                int len = hw.rand.nextInt(1050) + 1;
                int range = start + len;

                if(range > hw.imglength) continue;

                hw.muteFile = hw.mutate(inp, start, len);
                hw.executeTarget(hw.muteFile);
            }
            

       } catch(IOException e) {
         e.printStackTrace();
         System.out.println("Nothing..:] not mutated");
       }

    }
 
}

class LeitoresEscritores {
    private int leitor, escritor;  
    
    // Construtor
    LeitoresEscritores() { 
       this.escritor = 0;
       this.leitor = 0;
    } 
    
    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
      try { 
        while (this.escritor > 0) {
           System.out.println ("le.leitorBloqueado("+id+")");
           wait();   
        }
        this.leitor++;  
        System.out.println ("le.leitorLendo("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
       this.leitor--; 
       if (this.leitor == 0) 
             this.notify();
       System.out.println ("le.leitorSaindo("+id+")");
    }
    
    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
      try { 
        while ((this.leitor > 0) || (this.escritor > 0)) {
           System.out.println ("le.escritorBloqueado("+id+")");
           wait();  
        }
        this.escritor++;
        System.out.println ("le.escritorEscrevendo("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
       this.escritor--; 
       notifyAll(); 
       System.out.println ("le.escritorSaindo("+id+")");
    }
  }

class Leitor extends Thread {
    int id; //identificador da thread
    int sleep; //dormir por um tempo
    LeitoresEscritores monitor;//objeto monitor para coordenar a lógica de execução das threads
  
    // Construtor
    Leitor (int id, int sleep, LeitoresEscritores m) {
      this.id = id;
      this.sleep = sleep;
      this.monitor = m;
    }
  
    // Método executado pela thread
    public void run () {
      double j=777777777.7, i;
      try {
        for (;;) {
          this.monitor.EntraLeitor(this.id);
          for (i=0; i<100000000; i++) {j=j/2;}
          this.monitor.SaiLeitor(this.id);
          sleep(this.sleep); 
        }
      } catch (InterruptedException e) { return; }
    }
  }

class Escritor extends Thread {
    int id; 
    int sleep; 
    LeitoresEscritores monitor;
  
    // Construtor
    Escritor (int id, int sleep, LeitoresEscritores m) {
      this.id = id;
      this.sleep = sleep;
      this.monitor = m;
    }
  
    // Função que a thread vai executar
    public void run () {
      double j=777777777.7, i;
      try {
        for (;;) {
          this.monitor.EntraEscritor(this.id); 
          for (i=0; i<100000000; i++) {j=j/2;}
          this.monitor.SaiEscritor(this.id); 
          sleep(this.sleep); 
        }
      } catch (InterruptedException e) { return; }
    }
  }


public class App {
    static final int leit = 5;
    static final int esc = 4;
    public static void main (String[] args) {
        int i;
        LeitoresEscritores monitor = new LeitoresEscritores();
        Leitor[] l = new Leitor[leit];       
        Escritor[] e = new Escritor[esc];
    
        System.out.println ("import verificaLE");
        System.out.println ("le = verificaLE.LeitoresEscritores()");
        
        for (i=0; i<leit; i++) {
           l[i] = new Leitor(i+1, (i+1)*1000, monitor);
           l[i].start(); 
        }
        for (i=0; i<esc; i++) {
           e[i] = new Escritor(i+1, (i+1)*1000, monitor);
           e[i].start(); 
        }
      }
}
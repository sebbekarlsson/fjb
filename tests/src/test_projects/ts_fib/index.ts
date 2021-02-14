export class Fibonacci {
	index : number;
	fibonacciNumber : number;

  constructor() {
    this.index = 4;
  }
	
	public calculateFibonacciNumber() {		
		this.fibonacciNumber = this.calculate(this.index);						
	}
	
	private calculate(i : number) : number{
		return (i <= 2) ? 1 : this.calculate(i -1 ) + this.calculate(i -2);		
	}			
}

const fib:Fibonacci = new Fibonacci();

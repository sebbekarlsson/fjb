interface A { a: number }  
interface B { b: number }

var ab: A & B = { a: 1, b: 1 };  
var a: A = ab;  
var b: B = ab;

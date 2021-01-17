//const dot = 44;

function shouldNotBeHere() {

}

export function bepa() {
 // console.log(dot);
  apa();
  console.log("hello from bepa");
}

export function z() {
  console.log("hello from z");
  bepa();
}

export function noop() {
  console.log("hello from noop");
}

export function JohnDoe() {
  console.log("hello from John");
}

export const dotnet = function() {
  bepa();
}

export const apa = function() {
  console.log(1);
}

const bepa = 33;
const iShoudlNotBeeSeen = {};

function dontShowMe() {

}

export function interestingFunction(x, y) {
  return x * aDependencyFunction(y);
}

export function notSoInterestingFunction(x, y) {
  return x - y;
}

export function thisShouldNotBeHere() {

}

export function aDependencyFunction(x) {
  return anotherDependency(x) * 2;
}

export function anotherDependency(y)
{
  return y + nonExportedDependency(1);
}

function nonExportedDependency(k)
{
  return k * bepa;
}

//import { doSomething } from './helpers';
const password = require('secure-random-password');


console.log(password.randomPassword({ characters: password.lower, length: 4 }));

const x = 3 * 4;

if (x > 3) {
  console.log(x);
} else {
  console.log('hello');
};

const main = () => {
  doSomething(x);
};

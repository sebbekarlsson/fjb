import { partition } from 'lodash';

const x = partition([1, 2, 3, 4], n => n % 2);

console.log(x);

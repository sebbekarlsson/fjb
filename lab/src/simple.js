import _ from 'lodash';

const apa = _.partition([1, 2, 3, 4], n => n % 2);

console.log(apa);

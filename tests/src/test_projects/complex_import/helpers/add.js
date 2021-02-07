import { bug } from './bug';

export function add(x, y)
{
  bug(x);
  return x + y;
}

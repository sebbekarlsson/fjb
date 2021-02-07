import { bug } from './bug';


export function sub(x, y)
{
  bug(y);
  return x - y;
}

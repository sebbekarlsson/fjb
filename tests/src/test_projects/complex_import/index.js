import { add } from './helpers/add';
import { sub } from './helpers/sub';


function main()
{
  const x = 4;
  const y = 8;
  const z = add(x, y);
  const v = sub(z, x);

  console.log(`Output: ${v}`);
}

main();

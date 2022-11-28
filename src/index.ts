import dotenv from 'dotenv';
import db from './db';
import server from './server';

dotenv.config();

const PORT = process.env.PORT;

const start = async () => {
  console.log('[database] Running DB sync');
  await db.sync().catch((err) => {
    console.error('[database] Error running DB sync: ', err);
  });

  server.listen(PORT, async () => {
    // tslint:disable-next-line:no-console
    console.log(`[server] Running on port ${PORT}`);
  });
};

start();

import { Router } from 'express';

const router = Router();

router.get('/healthcheck', (_, res) => {
  res.send('OK');
});

export default router;

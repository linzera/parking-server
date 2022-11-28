import { Router } from 'express';
import { findOrCreateParkingData } from '../repository/parkingDataRepository';
import { io } from '../server';

const router = Router();

router.post('/parking', async (req, res) => {
  // tslint:disable-next-line:no-console
  console.log('[server] New parking information: ', req.body);

  const { id, isParked } = req.body;

  console.log(req.query);

  if (id === undefined) {
    return res.status(400).send({ error: 'Missing parameters' });
  }

  try {
    const [_, created] = await findOrCreateParkingData({
      id,
      isParked,
    });

    if (created) {
      console.log('[server] New parking hardware identified: ', id);
    }

    io.emit('parkData', { hardwareId: id, isParked });
  } catch (err) {
    console.log('[server] Error saving parking data: ', err);

    return res.status(500).send({ error: 'Error saving parking data' });
  }

  res.sendStatus(200);
});

export default router;

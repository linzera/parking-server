import express, { json } from 'express';
import path from 'path';

import healthcheckRoute from './routes/healthcheck';
import parkingRoute from './routes/parking';

const app = express();

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.use(json());
app.use(healthcheckRoute);
app.use(parkingRoute);

app.get('/', (req, res) => {
  res.render('index');
});

export default app;

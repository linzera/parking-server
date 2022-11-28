import { BOOLEAN, INTEGER, Model } from 'sequelize';
import db from '../db';

interface ParkingDataModelAttributes {
  hardwareId: number;
  isParked: boolean;
}

interface ParkingDataCreateAttributes {
  hardwareId: number;
  isParked: boolean;
}

export interface ParkingData {
  id: number;
  isParked: boolean;
}

const ParkingDetector = db.define<
  Model<ParkingDataModelAttributes, ParkingDataCreateAttributes>
>('parkingDetector', {
  hardwareId: {
    type: INTEGER,
    allowNull: false,
    primaryKey: true,
  },
  isParked: {
    type: BOOLEAN,
  },
});

export default ParkingDetector;

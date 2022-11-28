import model, { ParkingData } from '../model/parkingDetector';

export const findOrCreateParkingData = (parkingData: ParkingData) => {
  return model.findOrCreate({
    where: { hardwareId: parkingData.id },
    defaults: {
      hardwareId: parkingData.id,
      isParked: parkingData.isParked,
    },
  });
};

export const findAllParkingData = () => {
  return model.findAll();
};

export const saveParkingData = async (data: ParkingData) => {
  const newParking = await model.create({
    hardwareId: data.id,
    isParked: data.isParked,
  });

  if (!newParking) {
    throw new Error('[server] Error saving parking data');
  }

  return newParking;
};

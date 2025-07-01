
typedef struct {
    double latitude;
    double longitude;
    double altitude;
} NextPosition_t;

class DeadReckoning {
private:
    
public:
    DeadReckoning();
    ~DeadReckoning();

    static NextPosition_t PredictNextPosition(
        double latitude,         // degrees
        double longitude,        // degrees
        double altitude,         // in feet
        double speed,            // in meters per second
        double heading,          // in degrees (0 = North, clockwise)
        double verticalRate,     // in feet per minute
        double deltaTimeSec      // in seconds since last known position
    );

};

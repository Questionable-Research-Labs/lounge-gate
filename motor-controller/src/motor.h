enum MotorState { STATIONARY, FORWARDS, BACKWARDS };

struct tuned_value_result {
	MotorState direction;
	int motorOutput;
	float inputValue;
};
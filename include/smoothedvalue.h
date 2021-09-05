template<class T>
class SmoothedValue
{
public:
	SmoothedValue(T v, int steps = 64) : _v(v), _target(v), _delta(0), _ksteps(steps), _nsteps(0)
	{
	}
	~SmoothedValue()
	{
	}

	void setValue(T v)
	{
		_nsteps = 0;
		_target = v;
		_v = v;
		_delta = 0;
	}

	void setTargetValue(T target)
	{
		_nsteps = _ksteps;
		_target = target;
		_delta = (_target - _v) / (T)_nsteps;
	}

	T getValue() const
	{
		return _v;
	}

	T getTargetValue() const
	{
		return _target;
	}

	void tick()
	{
		if (_nsteps) {
			_v = _target - _delta * (T)_nsteps;
			_nsteps--;
		}
	}

	bool isStillSmoothing() const
	{
		return (_nsteps != 0);
	}
private:
	T _target;
	T _delta;
	T _v;
	int _nsteps;
	const int _ksteps;
};
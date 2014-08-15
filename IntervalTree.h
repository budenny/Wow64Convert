template <typename T>
struct Interval
{
	typedef T value_type;

	value_type start;
	value_type end;

	Interval(value_type start_, value_type end_) : start(start_), end(end_){};
	Interval(value_type start_): start(start_), end(start_ + 1){};

	inline bool operator <(const Interval & r) const {return start < r.start;}
	inline bool Contains(value_type v) const { return v >= start && v < end;}
};

template<typename T>
class IntervalTree
{
	typedef std::set<T> Container;
	Container _intervals;

public:

	void Insert(const T & item)
	{
		_intervals.insert(item);
	}

	const T * Find(const typename T::value_type & v) const
	{
		auto i = _intervals.lower_bound(T(v));
		if(i == _intervals.end() || v < i->start)
		{
			if(i == _intervals.begin())
				return nullptr;

			--i;
		}
		return i->Contains(v) ? &*i : nullptr;
	}
};
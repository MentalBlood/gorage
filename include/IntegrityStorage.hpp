#pragma once

#include <vector>
#include <memory>

#include "Usi.hpp"
#include "Storage.hpp"



namespace gorage {

	template<class T, class I>
	class IntegrityStorage : public Storage<T> {

	public:

		IntegrityStorage(std::shared_ptr<Storage<T>> base, std::shared_ptr<Storage<I>> integrity):
			_base(base),
			_integrity(integrity) {}

		void save(const Usi usi, const T content) {
			_integrity->save(digest_usi(usi), digest(content));
			_base->save(usi, content);
		}

		T load(const Usi usi) const {
			const T result = _base->load(usi);
			const I result_digest = digest(result);
			const I stated_digest = _integrity->load(digest_usi(usi));
			if (result_digest != stated_digest) {
				throw OperationalError("Can not load object with usi `" + usi() + "`: integrity check failed");
			}
			return result;
		}

		void remove(const Usi usi) {
			_base->remove(usi);
			_integrity->remove(digest_usi(usi));
		}

		virtual std::vector<Usi> usis() const {
			return _base->usis();
		}

	protected:

		virtual I digest(const T& content) const = 0;

		virtual Usi digest_usi(const Usi usi) const {
			return Usi(usi() + "_digest");
		}

	private:

		std::shared_ptr<Storage<T>> _base;
		std::shared_ptr<Storage<I>> _integrity;

	};

} // gorage
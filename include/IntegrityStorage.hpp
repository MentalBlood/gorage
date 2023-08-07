#pragma once

#include <vector>
#include <memory>

#include "Usi.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"



namespace gorage {

	namespace exceptions {

		class IntegrityError : public Base {
		public:
			IntegrityError(const gorage::Usi& usi):
				Base("Can not load object with usi `" + usi.value() + "`: integrity check failed") {}
		};

	}

	template<class T, class I>
	class IntegrityStorage : public Storage<T> {

	public:

		explicit IntegrityStorage(std::shared_ptr<Storage<T>> base, std::shared_ptr<Storage<I>> integrity):
			_base(base),
			_integrity(integrity) {}

		void save(const Usi& usi, const T& content) {
			_integrity->save(digest_usi(usi), digest(content));
			_base->save(usi, content);
		}

		T load(const Usi& usi) const {
			const T result        = _base->load(usi);
			const I result_digest = digest(result);
			const I stated_digest = _integrity->load(digest_usi(usi));
			if (result_digest != stated_digest) {
				throw exceptions::IntegrityError(usi);
			}
			return result;
		}
		Bytes raw(const Usi& usi) const {
			const Bytes result    = _base->raw(usi);
			const I result_digest = digest_raw(result);
			const I stated_digest = _integrity->load(digest_usi(usi));
			if (result_digest != stated_digest) {
				throw exceptions::IntegrityError(usi);
			}
			return result;
		}


		bool check(const Usi& usi) const {
			for (const Usi& usi : usis()) {
				if (
					digest_raw(_base->raw(usi))
					!=
					_integrity->load(digest_usi(usi))
				) {
					return false;
				}
			}
			return true;
		}

		void recalculate(const Usi& usi) const {
			_integrity->save(
				digest_usi(usi),
				digest(
					_base->load(usi)
				)
			);
		}

		virtual bool exists(const Usi& usi) const { return
			_base->exists(usi) && _integrity->exists(digest_usi(usi));
		}

		void remove(const Usi& usi) {
			_base->remove(usi);
			_integrity->remove(digest_usi(usi));
		}

		virtual std::vector<Usi> usis() const {
			return _base->usis();
		}

	protected:

		virtual I digest(const T& content) const = 0;
		virtual I digest_raw(const Bytes& content) const = 0;

		virtual Usi digest_usi(const Usi& usi) const {
			if constexpr (std::is_same_v<T, I>) {
				if (_base == _integrity) {
					return Usi(usi.value() + "_digest");
				}
			}
			return usi;
		}

	private:

		std::shared_ptr<Storage<T>> _base;
		std::shared_ptr<Storage<I>> _integrity;

	};

} // gorage